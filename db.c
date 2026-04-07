#include "db.h"
#include <stdio.h>

int initialize_db(sqlite3 **db){
  int out = sqlite3_open_v2("study-budget.db",
                           db,
                           SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                           NULL);
  if (out){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(*db));
    sqlite3_close(*db);
    return 1;
  } else {return 0;}
}
