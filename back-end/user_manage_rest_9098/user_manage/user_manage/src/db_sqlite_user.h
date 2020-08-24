#if !defined(DB_SQLITE_USER_H_)
#define DB_SQLITE_USER_H_
#include <string>

int db_sqlite_insert_user(std::string _id, std::string _pwd, std::string _username);

bool db_sqlite_query_user(std::string _id, std::string _pwd);

std::string db_sqlite_logon_user(std::string _id);



#endif // DB_SQLITE_USER_H_
