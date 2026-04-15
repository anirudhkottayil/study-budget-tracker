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
  strncpy(tasks[index].task,(const char*) sqlite3_column_text(ppStmt, 1), 49);
  tasks[index].task[49] = '\0';
  tasks[index].estimated_mins = sqlite3_column_int (ppStmt, 2);
  tasks[index].observed_mins = sqlite3_column_int (ppStmt, 3);
}

void map_logs(sqlite3_stmt* ppStmt, void* arr, int index){
  Daily_logs* logs = (Daily_logs*) arr;
  strncpy(logs[index].date,(const char*) sqlite3_column_text(ppStmt, 0), 12);
  logs[index].date[12] = '\0';
  logs[index].sleep_time = sqlite3_column_int (ppStmt, 1);
  logs[index].wake_time= sqlite3_column_int (ppStmt, 2);
  logs[index].sleep_dur_min = sqlite3_column_int (ppStmt, 3);
  logs[index].sleep_quality = sqlite3_column_int (ppStmt, 4);
  logs[index].mood = sqlite3_column_int (ppStmt, 5);
  logs[index].energy = sqlite3_column_int (ppStmt, 6);
  logs[index].eat_out_meals = sqlite3_column_int (ppStmt, 7);
  logs[index].home_cooked = sqlite3_column_int (ppStmt, 8);
  logs[index].steps = sqlite3_column_int (ppStmt, 9);
  logs[index].outside_min = sqlite3_column_int (ppStmt, 10);
  logs[index].screen_time_min = sqlite3_column_int (ppStmt, 11);
  logs[index].exercise = sqlite3_column_int (ppStmt, 12);
  logs[index].caffeine_drinks = sqlite3_column_int (ppStmt, 12);
  logs[index].stress = sqlite3_column_int (ppStmt, 13);
  logs[index].productive_feel = sqlite3_column_int (ppStmt, 14);
  strncpy(logs[index].notes, (const char*) sqlite3_column_text (ppStmt, 15), 49);
  logs[index].notes[49] = '\n';
}
