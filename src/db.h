#ifndef DB_H
#define DB_H
#include <sqlite3.h>

typedef struct {
  int id;
  char subject[50];
} Subjects;

typedef struct {
  int id;
  int estimated_mins;
  int observed_mins;
  char task[50];
} Task;

typedef struct {
  char date[13];
  int sleep_time;
  int wake_time;
  int sleep_dur_min;
  int sleep_quality;
  int mood;
  int energy;
  int eat_out_meals;
  int home_cooked;
  int steps;
  int outside_min;
  int screen_time_min;
  int exercise;
  int caffeine_drinks;
  int stress;
  int productive_feel;
  char notes[50];
} Daily_logs;

typedef void (Rowmapper) (sqlite3_stmt* ppStmt, void* arr, int index);

int initialize_db(sqlite3 **db);
int first_write_into_db(sqlite3* db, int arr[], char** sub_arr);
int insert_into_db();
int update_db();
int show_db();
int free_db();
int count_rows(sqlite3* db, const char* statement);
int get_rows(sqlite3* db, const char* statement, void* arr, Rowmapper mapper, int* params, int param_length);
int sql_command_exec(sqlite3 *db, char* tablename, const char* command, int* arr,int arr_length, char* text);
int first_run(sqlite3 *db);
#endif
