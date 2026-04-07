#ifndef DB_H
#define DB_H
#include <sqlite3.h>

int initialize_db(sqlite3 **db);
int insert_into_db();
int update_db();
int show_db();
int free_db();

#endif
