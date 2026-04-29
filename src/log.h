#ifndef LOG_H
#define LOG_H

#include <sqlite3.h>
#include "db.h"

int check_first_time_run();
int first_time_data(sqlite3* db);
void free_memory(sqlite3* db, Subjects* subjects,Task* task);
char* check_new_day(sqlite3* db);
int first_start_day(sqlite3* db, char* date);


#endif
