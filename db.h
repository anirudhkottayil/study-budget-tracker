#ifndef DB_H
#define DB_H
#include <sqlite3.h>

typedef struct {
  int id;
  char* subject;
} Subjects;

int initialize_db(sqlite3 **db);
int first_write_into_db(sqlite3* db, int arr[], char** sub_arr);
int insert_into_db();
int update_db();
int show_db();
int free_db();
int count_subjects(sqlite3* db);
int get_subjects(sqlite3* db, Subjects* subject);

#endif
