#include "db_sqlite_user.h"
#include <sqlite3.h>
bool db_sqlite_insert_user(std::string _id, std::string _pwd, std::string _username)
{
    int db_ret = -1;
    bool bRet = false;
    sqlite3 *db = NULL;

    db_ret = sqlite3_open("/database/pk_user", &db);
    if (0 == db_ret)
    {
        std::string sql_cmd = "insert into pk_user values ('" + _id + "', '" + _pwd + "', '" + _username + "');";
        db_ret = sqlite3_exec(db, sql_cmd.c_str(), NULL, NULL, NULL);
        if (SQLITE_OK ==  db_ret)
        {
            bRet = true;
        }
    }

    return bRet;
}