#include "db_sqlite_user.h"
#include "random_user.h"
#include <sqlite3.h>
#include <string.h>
#include <hiredis/hiredis.h>
#include "Base64.h"

static bool id_is_valid(std::string _id)
{
    bool bRet = false;

    if (_id.length() == 11)
    {
        bool isallDig = true;
        for (auto var : _id)
        {
            if (!isdigit(var))
            {
                isallDig = false;
                break;
            }
        }
        if (isallDig)
        {
            bRet = true;
        }
    }

    return bRet;
}
static bool pwd_is_valid(std::string _pwd)
{
    bool bRet = false;

    if (_pwd.length() > 0)
    {
        bRet = true;
    }

    return bRet;
}
static bool username_is_valid(std::string _username)
{
    bool bRet = false;

    if (_username.length() > 0)
    {
        bRet = true;
    }

    return bRet;
}

int db_sqlite_insert_user(std::string _id, std::string _pwd, std::string _username)
{
    int db_ret = -1;
    int bRet = 2;
    sqlite3 *db = NULL;

    bool bInputValid = true;
    bInputValid &= id_is_valid(_id);
    bInputValid &= pwd_is_valid(_pwd);
    bInputValid &= username_is_valid(_username);

    if (bInputValid)
    {
        db_ret = sqlite3_open("/database/pk_user", &db);
        if (0 == db_ret)
        {
            char *errmsg = NULL;
            std::string sql_cmd = "insert into pk_user values ('" + _id + "', '" + _pwd + "', '" + _username + "', 0);";
            db_ret = sqlite3_exec(db, sql_cmd.c_str(), NULL, NULL, &errmsg);
            if (SQLITE_OK == db_ret)
            {
                bRet = 0;
            }
            else
            {
                std::string errstring(errmsg);
                if (errmsg == std::string("UNIQUE constraint failed: pk_user.id"))
                {
                    bRet = 1;
                }
                sqlite3_free(errmsg);
            }

            sqlite3_close(db);
        }
    }

    return bRet;
}

struct db_query_user{
    const char *pwd;
    int number;
};
static int verify_user_password(void *_pQA, int agrc, char ** argv, char **col)
{
    int ret = -1;
    db_query_user *pQueryArg = (db_query_user *)_pQA;

    if (0 == strcmp(pQueryArg->pwd, argv[0]))
    {
        pQueryArg->number = 1;
        ret = 0;
    }

    return ret;
}

bool db_sqlite_query_user(std::string _id, std::string _pwd)
{
    bool bRet = false;
    sqlite3 *db = NULL;

    auto dbret = sqlite3_open("/database/pk_user", &db);
    if (SQLITE_OK == dbret)
    {
        std::string sql_cmd = "select pwd from pk_user where id = '" + _id + "';";
        db_query_user query_arg = {_pwd.c_str(), 0};
        dbret = sqlite3_exec(db, sql_cmd.c_str(), verify_user_password, (void *)&query_arg, NULL);
        if (SQLITE_OK == dbret)
        {
            if (query_arg.number == 1)
            {
                bRet = true;
            }
        }

        sqlite3_close(db);
    }


    return bRet;
}

std::string get_ssid_by_id(std::string _id)
{
    std::string ret = "";

    redisContext *predis = redisConnect("localhost", 6379);
    if (NULL != predis)
    {
        auto preply = redisCommand(predis, "HGET id:%s ssid", _id.c_str());
        if (NULL != preply)
        {
            auto pStdReply = (redisReply *)preply;
            if (pStdReply->type == REDIS_REPLY_STRING)
            {
                if (pStdReply->len == 32)
                {
                    ret.assign(pStdReply->str, pStdReply->len);
                }
            }
            freeReplyObject(preply);
        }

        redisFree(predis);
    }

    return ret;
}

bool store_ssid_id(std::string _id, std::string _ssid, std::string _name, int _cash)
{
    bool bRet = false;

    redisContext *predis = redisConnect("localhost", 6379);
    if (NULL != predis)
    {
        auto preply = redisCommand(predis, "HMSET user_ssid:%s name %s cash %d id %s", _ssid.c_str(), _name.c_str(), _cash, _id.c_str());
        if (NULL != preply)
        {
            freeReplyObject(preply);
            preply = redisCommand(predis, "HMSET id:%s ssid %s", _id.c_str(), _ssid.c_str());
            if (NULL != preply)
            {
                freeReplyObject(preply);
                bRet = true;
            }
            else
            {
                preply = redisCommand(predis, "DEL user_ssid:%s", _ssid.c_str());
                if (NULL != preply)
                {
                    freeReplyObject(preply);
                }
            }

        }
        redisFree(predis);
    }


    return bRet;
}

int logon_callback(void *_pri_data, int argc, char **argv, char **columns)
{
    std::string *pArg = (std::string *)_pri_data;
    int bRet = -1;
    std::string id = "";
    std::string name = "";
    int cash = 0;

    for (int i = 0; i < argc; i++)
    {
        if (0 == strcasecmp("id", columns[i]))
        {
            id = argv[i];
        }
        if (0 == strcasecmp("name", columns[i]))
        {
            name = argv[i];
        }
        if (0 == strcasecmp("cash", columns[i]))
        {
            cash = atoi(argv[i]);
        }
    }

    *pArg = get_ssid_by_id(id);
    if (pArg->length() == 32)
    {
        bRet = 0;
    }
    else
    {
        auto ssid = GetRanSsid();
        if (true == store_ssid_id(id, ssid, name, cash))
        {
            *pArg = ssid;
            bRet = 0;
        }
    }

    return bRet;
}

std::string db_sqlite_logon_user(std::string _id)
{
    sqlite3 *db;
    std::string ssid_ret = "";

    auto db_ret = sqlite3_open("/database/pk_user", &db);
    if (SQLITE_OK == db_ret)
    {
        std::string sql_cmd = "select * from pk_user where id = '" + _id + "';";

        sqlite3_exec(db, sql_cmd.c_str(), logon_callback, &ssid_ret, NULL);

        sqlite3_close(db);
    }

    return ssid_ret;
}