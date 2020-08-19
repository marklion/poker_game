#include "db_sqlite_user.h"
#include <sqlite3.h>
#include <string.h>

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
            std::string sql_cmd = "insert into pk_user values ('" + _id + "', '" + _pwd + "', '" + _username + "');";
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

std::string db_sqlite_logon_user(std::string _id)
{
    return "246a105e50fc4e6883ebb4b8c558d16a";
}