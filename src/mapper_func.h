#ifndef MAPPER_FUNC_H
#define MAPPER_FUNC_H
#include <sqlite3.h>

void map_subject(sqlite3_stmt* ppStmt, void* arr, int index);
void map_tasks(sqlite3_stmt* ppStmt, void* arr, int index);
void map_logs(sqlite3_stmt* ppStmt, void* arr, int index);
void map_expense(sqlite3_stmt *ppStmt, void *arr, int index); 
void map_income_event(sqlite3_stmt *ppStmt, void *arr, int index);


#endif
