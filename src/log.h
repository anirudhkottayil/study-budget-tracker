#ifndef LOG_H
#define LOG_H

#include <sqlite3.h>
#include "db.h"

int first_start();
int first_daily_log();
int first_budget_log();

int display_menu();

int display_study_menu();
int start_study();
int stop_study();
int show_study_logs(int num);

int display_daily_log_menu();
int show_daily_logs(int num);
int update_daily_log();

int display_expenses_menu();
int show_expenses(int num);
int update_expenses();

int display_subjects_menu();
int update_subjects();

int display_income_menu();
int show_income_logs(int num);
int update_income();

int check_first_time_run();
int first_time_data(sqlite3* db);
void free_memory(sqlite3* db, Subjects* subjects,Task* task);
char* check_new_day(sqlite3* db);
int first_start_day(sqlite3* db, char* date);


#endif
