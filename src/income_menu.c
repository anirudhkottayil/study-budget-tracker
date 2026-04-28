#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "income_menu.h"
#include "db.h"
#include "sql_commands.h"
#include "mapper_func.h"
#include "utils.h"
#include "constants.h"

int get_income_input(int* arr, char *notes) {
  double amount = read_double_input("Enter amount: ", 1.0, 2147483647.0);
  arr[0] = (int)round(amount * 100);

  printf("| %-3s | %-10s |\n", "ID", "SOURCE");
  printf("|-----|------------|\n");
  for (int i = 0; i < 4; i++)
      printf("| %-3d | %-10s |\n", i, sources[i]);
  arr[1] = read_int_input("Enter source: ", 1, 4) - 1;
  printf("Enter notes: ");
  fgets(notes, 50, stdin);
  notes[strcspn(notes, "\n")] = '\0';

  return 0;
}

int update_event(sqlite3* db, char* date, int* in_study){
  int rc = 1; int arr[2]; char notes[50];
  if (*in_study){
    printf("IN STUDY SESSION\n");
  }

  get_income_input(arr, notes);
  
  rc = sql_command_exec(db, update_income, arr, 2, notes, date);
  if (rc){
    fprintf(stderr, "income update failed\n");
    return -1;
  }
  if (sqlite3_changes(db) == 0) {
    printf("No income record found for date: %s\n", date);
    return 1;
  }


  return 0;
}


int insert_event(sqlite3* db, int* in_study){
  int rc = 1; int arr[2]; char notes[50];

  get_income_input(arr, notes);

  rc = sql_command_exec(db, insert_income, arr, 2, notes, NULL);
  if (rc) return 1;  
  char date[11];
  time_t t = time(NULL);
  struct tm *tp = localtime(&t);
  sprintf(date, "%04d-%02d-%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);

  rc = bank_snapshot(db, date);
  if (rc) return 1;
  return 0;
}

void print_events(IncomeEvent* events, int count, int* in_study){
  if (*in_study){
    printf("IN STUDY SESSION\n");
  }
  const char *sources[] = { "Salary", "Freelance", "Transfer", "Other" };

  for (int i = 0; i < count; i++) {
      int src = events[i].source;
      printf("┌─────────────────────────────────────┐\n");
      printf("│  ID:      %d\n",    events[i].id);
      printf("│  Date:    %s\n",    events[i].date);
      printf("│  Amount:  $%.2f\n", events[i].amount_cents / 100.0);
      printf("│  Source:  %s\n",    (src >= 0 && src <= 3) ? sources[src] : "Unknown");
      printf("│  Notes:   %s\n",    events[i].notes[0] ? events[i].notes : "—");
      printf("└─────────────────────────────────────┘\n");
  }
}
int count_income(sqlite3* db){
  int db_rows = count_rows(db, count_income_events);
  if ( db_rows == -1){
    fprintf(stderr, "Error getting row count for income logs\n");
    return -1;
  }

  return db_rows;
}

int view_income_logs(sqlite3* db, int input, int* in_study){
  int arr[1] = {input};
  IncomeEvent* event = malloc(arr[0] * sizeof(IncomeEvent));
  int rc = get_rows(db, get_income_events,(void*) event, map_income_event , arr, 1) ;
  if (rc == -1){
    fprintf(stderr, "Error getting rows for expenses\n");
    return 1;
  }

  print_events(event, arr[0], in_study);
  free(event);

  return 0;
}


int income_menu(sqlite3* db, int* in_study){

  int user_input ; int rc; int if_input;
  while (1){
    if (*in_study){
      printf("__INCOME_MENU___ (IN STUDY SESH)\n");
    } else {
      printf("___INCOME_MENU___\n");
    }
    printf("Enter 1 to view income logs\n");
    printf("Enter 2 to insert an income log\n");
    printf("Enter 3 to update an income log\n");
    printf("Enter 4 to go back to main menu\n");
    user_input = read_int_input("Enter your choice: ", 1, 4);

    if (user_input == 1){
      int count_logs = count_income(db);
      if (count_logs == -1) return 1;
      if (count_logs == 0){
        printf("No logs to show\n");
      } else {
        if_input = read_int_input("How many previous days income logs do you want to see : ", 1, count_logs);
        rc = view_income_logs(db, if_input, in_study);
        if (rc) {
          return 1;
        }
      }
    } else if (user_input == 2){
      rc = insert_event(db,in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 3){
      char* date = read_date_input("Enter the date of the income event you want to update: ");
      rc = update_event(db, date, in_study);
      if (rc == 1) {
        free(date);
        printf("Try again ?\n");
      } else if (rc == -1){
        free(date);
        return 1;
      } else {
        printf("Success!\n");
        rc = bank_snapshot(db, date);
        free(date);
        if (rc) return 1;
      }

    } else if (user_input == 4){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }


  return 0;
}
