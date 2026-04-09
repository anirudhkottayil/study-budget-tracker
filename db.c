#include "db.h"
#include <stdio.h>
#include "schema.h"

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

int first_run(sqlite3 *db){
  sqlite3_stmt *ppStmt = NULL;
  const char *pZtail = NULL;
  const char* sql = DB_SCHEMA;
  int rc;
  while( sql[0] != '\0') {
    rc = sqlite3_prepare_v2(db,sql, -1,&ppStmt, &pZtail);
    if (rc != SQLITE_OK){ return rc; }

    if (ppStmt != NULL){
      rc = sqlite3_step(ppStmt);
      sqlite3_finalize(ppStmt);
      if (rc != SQLITE_DONE){ return rc; }
    }
    if (pZtail == sql){ break; }
    sql = pZtail;
  }
  return SQLITE_OK;
}

int first_write_into_db(sqlite3* db, int arr[], char** sub_arr){
  
}
