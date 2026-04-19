#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "income_menu.h"
#include "db.h"
#include "sql_commands.h"
#include "mapper_func.h"

int get_income_input(int arr[], char *notes) {
  const char *sources[] = { "Salary", "Freelance", "Transfer", "Other" };

  printf("Enter amount in cents (e.g. 150000 = $1500.00): ");
  scanf("%d", &arr[0]);

  printf("| %-3s | %-10s |\n", "ID", "SOURCE");
  printf("|-----|------------|\n");
  for (int i = 0; i < 4; i++)
      printf("| %-3d | %-10s |\n", i, sources[i]);
  printf("Enter source: ");
  scanf("%d", &arr[1]);

  getchar();
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
  
  rc = sql_command_exec(db, "income_events", update_income, arr, 2, notes, date);
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

  rc = sql_command_exec(db, "income_events", insert_income, arr, 2, notes, NULL);
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

int view_income_logs(sqlite3* db, int input, int* in_study){
  int db_rows = count_rows(db, count_income_events);
  if ( db_rows == -1){
    fprintf(stderr, "Error getting row count for income logs\n");
    return 1;
  }
  int arr[1]= {(input > db_rows) ? db_rows : input};
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
    scanf("%d", &user_input);
    getchar();

    if (user_input == 1){
      printf("How many previous days income logs do you want to see : ");
      scanf("%d", &if_input);
      getchar();
      printf("\n");
      rc = view_income_logs(db, if_input, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 2){
      rc = insert_event(db, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 3){
      char date[11];
      printf("Enter the date of the income event you want to update: ");
      scanf("%10s", date);
      getchar();
      date[10] = '\0';
      printf("\n");
      rc = update_event(db, date, in_study);
      if (rc == 1) {
        printf("Try again ?\n");
      } else if (rc == -1){
        return 1;
      } else {
        printf("Success!\n");
      }

    } else if (user_input == 4){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }


  return 0;
}
