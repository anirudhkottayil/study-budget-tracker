#include "db.h"
#include <string.h>

void map_subject(sqlite3_stmt* ppStmt, void* arr, int index){
  Subjects* subjects = (Subjects*) arr;
      subjects[index].id = sqlite3_column_int (ppStmt, 0);
      strncpy(subjects[index].subject,(const char*) sqlite3_column_text(ppStmt, 1), 49);
      subjects[index].subject[49] = '\0'; 
}

void map_tasks(sqlite3_stmt* ppStmt, void* arr, int index){
  Task* tasks = (Task*) arr;
      tasks[index].id = sqlite3_column_int (ppStmt, 0);
      tasks[index].estimated_mins = sqlite3_column_int (ppStmt, 1);
      tasks[index].observed_mins = sqlite3_column_int (ppStmt, 2);
      strncpy(tasks[index].task,(const char*) sqlite3_column_text(ppStmt, 3), 49);
      tasks[index].task[49] = '\0';
}
