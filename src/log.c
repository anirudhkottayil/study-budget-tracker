#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "db.h"
// #include "expense_menu.h"
#include "sql_commands.h"
#include "utils.h"
// #include "constants.h"


char* check_new_day(sqlite3* db){
  char date_today[11];
  char* prev_date = malloc(11*sizeof(char));
  prev_date[0] = '\0';
  int rc = get_prev_date(db, prev_date);
  if (rc){
    free(prev_date);
    fprintf(stderr, "Error getting previous day date\n");
    return NULL;
  }
  time_t t = time(NULL);
  strftime(date_today, sizeof(date_today), "%Y-%m-%d", localtime(&t));
  if (strcmp(date_today, prev_date) == 0){
    free(prev_date);
    return NULL;
  } else {
    return prev_date;
  }
}

void get_daily_log_eod(int arr[], char *notes) {
  arr[0] = read_int_input("Eat out meals (0-3): ", 0, 3);
  arr[1] = read_int_input("Home cooked? (1 = yes, 0 = no): ", 0, 1);
  arr[2] = read_int_input("Steps: ", 1, 65000);
  arr[3] = read_int_input("Outside time (mins): ", 0, 1080);
  arr[4] = read_int_input("Screen time (mins): ", 0, 1080);
  arr[5] = read_int_input("Exercise? (1 = yes, 0 = no): ", 0, 1);
  arr[6] = read_int_input("Caffeine drinks: ", 0, 5);
  arr[7] = read_int_input("Stress (1-5): ", 0, 5);
  arr[8] = read_int_input("Enter productive feel: ", 1, 5);
  printf("Notes: ");
  fgets(notes, 50, stdin);
  notes[strcspn(notes, "\n")] = '\0';
  printf("\n");
}


int get_expense_loop(ExpenseEntry** entries, int* count){
  int capacity = *count;
  *count = 0;
  char sentinel[4];

  while(1) {
    printf("Add expense ? (press q to finish anyother key to continue): ");
    scanf("%3s", sentinel);
    getchar();
    printf("\n");

    if (sentinel[0] == 'q') {
      break;
    }
    if (*count == capacity) {
      capacity *= 2;
      ExpenseEntry* tmp = realloc(*entries, capacity * sizeof(ExpenseEntry));
      if (tmp == NULL){
        fprintf(stderr, "Error reallocating memory for expense entry\n");
        return 1;
      }
      *entries = tmp;
    }
    get_expense_input((*entries)[*count].arr, (*entries)[*count].notes);
    (*count)++;
  }
  return 0;
}

int first_start_day(sqlite3* db, char* date){
  int daily_arr[9]; char log_notes[50]; int rc;

  printf("Enter previous days daily log and expenses\n");
  get_daily_log_eod(daily_arr, log_notes);
  rc = sql_command_exec(db, update_eod_log, daily_arr, 9, log_notes, date);
  if (rc) return 1;
  int count = 4;
  ExpenseEntry *entry = malloc(count * sizeof(ExpenseEntry));
  rc = get_expense_loop(&entry, &count);
  if (rc) {free(entry); return 1;}
  rc = expense_entry(db, entry, count, date);
  if (rc) {free(entry); return 1;}
  rc = bank_snapshot(db, date);
  if (rc) {free(entry); return 1;}
  free(entry);
  return 0;
}


int check_first_time_run(){
  FILE *file = fopen(".appran", "r");
  if (file){
    fclose(file);
    return 0;
  } else{
    file = fopen(".appran", "w");
    fclose(file);
    return 1;
  }
}

int first_time_data(sqlite3* db){
  int arr[8]; double amount;
  arr[0] = read_int_input("Enter sleep time (24hr format): ", 0000, 2359);
  arr[1] = read_int_input("Enter wake time (24hr format): ", 0000, 2359);
  //arr[2] computed below
  arr[3] = read_int_input("Enter sleep quality: ", 1, 5);
  arr[4] = read_int_input("Enter mood: ", 1, 5);
  arr[5] = read_int_input("Enter energy: ", 1, 5);
  amount = read_double_input("Enter your bank balance: ", 1.0, 600000000);
  arr[6] = (int)round(amount * 100);
  arr[7] = read_int_input("Enter how many subjects do you want: ", 1, 10);
  char** sub_arr = malloc (arr[7]* sizeof(char*));
  for (int i = 0; i < arr[7]; i++){
    sub_arr[i] = malloc(50 * sizeof(char));
    fgets( sub_arr[i], 50*sizeof(char), stdin);
    sub_arr[i][strcspn(sub_arr[i], "\n")] = '\0';
  }
  int sleep_h = arr[0] / 100;
  int sleep_m = arr[0] % 100;
  int wake_h  = arr[1] / 100;
  int wake_m  = arr[1] % 100;

  int sleep_total = sleep_h * 60 + sleep_m;
  int wake_total  = wake_h  * 60 + wake_m;

  int duration;
  if (wake_total < sleep_total){
      // crossed midnight
      duration = (1440 - sleep_total) + wake_total;
  } else {
      duration = wake_total - sleep_total;
  }
  arr[2] = duration;
  first_write_into_db(db, arr, sub_arr);
  for (int i = 0; i < arr[7]; i++) free(sub_arr[i]);
  free(sub_arr);
  return 0;
}


void free_memory(sqlite3* db, Subjects* subjects, Task* task){
  sqlite3_close(db);
  if (subjects != NULL) free(subjects);
  if (task != NULL) free(task);
}
