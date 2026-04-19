#include "db.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "schema.h"
#include "sql_commands.h"


int get_row_date(sqlite3* db, const char* statement, void* arr, Rowmapper mapper, char* date){

  sqlite3_stmt *ppStmt = NULL;
  if (sqlite3_prepare_v2(db, statement, -1, &ppStmt, NULL) != SQLITE_OK){
    fprintf(stderr, "Couldn't get rows: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_text(ppStmt, 1, date, -1, SQLITE_STATIC);

  if (sqlite3_step(ppStmt) == SQLITE_ROW)
    mapper(ppStmt, arr, 0);

  
  sqlite3_finalize(ppStmt);
  return 0;

}

int get_rows(sqlite3* db, const char* statement, void* arr, Rowmapper mapper, int* params, int param_length){
  sqlite3_stmt *ppStmt = NULL;
  if (sqlite3_prepare_v2(db, statement, -1, &ppStmt, NULL) != SQLITE_OK){
    fprintf(stderr, "Couldn't get rows: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  for (int i = 0; i < param_length; i++){
    sqlite3_bind_int(ppStmt,i+1, params[i]);
  }

  int i = 0;
  while (sqlite3_step(ppStmt) == SQLITE_ROW) {
    mapper(ppStmt, arr, i);
    i++;
  }
  sqlite3_finalize(ppStmt);

  return i;
}

int count_rows(sqlite3* db, const char* statement){
  sqlite3_stmt *ppStmt = NULL;
  int count = 0;

  if (sqlite3_prepare_v2(db, statement, -1, &ppStmt, NULL) != SQLITE_OK){
    fprintf(stderr, "Couldnt count subjects: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  if (sqlite3_step(ppStmt) == SQLITE_ROW){
    count = sqlite3_column_int(ppStmt, 0);
  }
  sqlite3_finalize(ppStmt);

  return count;
}
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

int sql_command_exec(sqlite3 *db, char* tablename, const char* command, int* arr, int arr_length, char* text, char* date){
  sqlite3_stmt *ppStmt = NULL;
  const char *pZtail = NULL;
  const char* sql = command;
  int rc;
  while( sql[0] != '\0') {
    rc = sqlite3_prepare_v2(db,sql, -1,&ppStmt, &pZtail);
    if (rc != SQLITE_OK){ return rc; }

    int i = 0;
    for (i = 0; i < arr_length; i++){
      sqlite3_bind_int(ppStmt, i+1, arr[i]);
    }
    if (text != NULL){
      int text_len = text ? strlen(text) : 0;
      sqlite3_bind_text(ppStmt, i+1, text, text_len, SQLITE_STATIC);
      i++;
    }
    if (date != NULL){
      sqlite3_bind_text(ppStmt, i+1, date, -1, SQLITE_STATIC);

    }

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

  sqlite3_stmt *ppStmt = NULL;

  if (sqlite3_prepare_v2(db, first_log, -1, &ppStmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error on first insert to log: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  for (int i = 0; i < 5; i++){
    sqlite3_bind_int(ppStmt, i + 1, arr[i]);
  }

  if (sqlite3_step(ppStmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to insert first log: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(ppStmt);
    return -1;
  }

  sqlite3_finalize(ppStmt);

  if (sqlite3_prepare_v2(db, first_bank_balance, -1, &ppStmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error on first insert to bank balance: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  sqlite3_bind_int(ppStmt, 1, arr[5]);
  sqlite3_bind_int(ppStmt, 2, arr[5]);

  if (sqlite3_step(ppStmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to insert first bank balance: %s\n", sqlite3_errmsg(db));
    sqlite3_finalize(ppStmt);
    return -1;
  }

  sqlite3_finalize(ppStmt);

  if (sqlite3_prepare_v2(db,insert_subjects, -1, &ppStmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Error on first insert to subjects: %s\n", sqlite3_errmsg(db));
    return -1;
  }

  for ( int i = 0 ; i < arr[6]; i++){
    sqlite3_bind_text(ppStmt, 1, sub_arr[i], -1, SQLITE_STATIC);

     if (sqlite3_step(ppStmt) != SQLITE_DONE) {
        fprintf(stderr, "Insert subject failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(ppStmt);
        return -1;
    }

    sqlite3_reset(ppStmt);
    sqlite3_clear_bindings(ppStmt);

  }
    sqlite3_finalize(ppStmt);

  return 0;
}
