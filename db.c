#include "db.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "schema.h"
#include "sql_commands.h"

int get_subjects(sqlite3* db, Subjects* subjects){
  sqlite3_stmt *ppStmt = NULL;
  sqlite3_prepare_v2(db, "SELECT id, name FROM subjects;", -1, &ppStmt, NULL);
  int i = 0;
  while (sqlite3_step(ppStmt) == SQLITE_ROW) {
      subjects[i].id = sqlite3_column_int (ppStmt, 0);
      subjects[i].subject = malloc(sizeof(char) * 50);
      strncpy(subjects[i].subject,(const char*) sqlite3_column_text(ppStmt, 1), 49);
      subjects[i].subject[49] = '\0'; 
      i++;
  }

  sqlite3_finalize(ppStmt);

  return 0;
}

int count_subjects(sqlite3* db){
  sqlite3_stmt *ppStmt = NULL;
  int count = 0;
  const char* statement = "SELECT COUNT(*) FROM subjects;";

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

int sql_command_exec(sqlite3 *db, char* tablename, const char* command, int* arr, int arr_length, char* text){
  sqlite3_stmt *ppStmt = NULL;
  const char *pZtail = NULL;
  const char* sql = command;
  int rc;
  while( sql[0] != '\0') {
    rc = sqlite3_prepare_v2(db,sql, -1,&ppStmt, &pZtail);
    if (rc != SQLITE_OK){ return rc; }

    int i;
    for (i = 0; i < arr_length; i++){
      sqlite3_bind_int(ppStmt, i+1, arr[i]);
    }
    int text_len = text ? strlen(text) : 0;
    sqlite3_bind_text(ppStmt, i+1, text, text_len, SQLITE_STATIC);

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
