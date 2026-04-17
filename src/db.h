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

typedef struct {
    int id;
    char date[11];
    int category;
    int amount_cents;
    int need_score;
    int want_score;
    int importance;
    int recurrence;
    int planned;
    int payment_method;
    int time_of_purchase;
    char notes[50];
} Expense;

typedef struct {
  int id;
  char date[11];
  int amount_cents;
  int source;
  char notes[50];
} IncomeEvent;

typedef void (Rowmapper) (sqlite3_stmt* ppStmt, void* arr, int index);

int initialize_db(sqlite3 **db);
int first_write_into_db(sqlite3* db, int arr[], char** sub_arr);
int insert_into_db();
int update_db();
int show_db();
int free_db();
int count_rows(sqlite3* db, const char* statement);
int get_rows(sqlite3* db, const char* statement, void* arr, Rowmapper mapper, int* params, int param_length);
int sql_command_exec(sqlite3 *db, char* tablename, const char* command, int* arr,int arr_length, char* text, char* date);
int first_run(sqlite3 *db);
int get_row_date(sqlite3* db, const char* statement, void* arr, Rowmapper mapper, char* date);

#endif
