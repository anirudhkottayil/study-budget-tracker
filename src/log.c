#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "db.h"
#include "expense_menu.h"
#include "sql_commands.h"


char* check_new_day(sqlite3* db){
  char date_today[11];
  char* prev_date = malloc(11*sizeof(char));
  int rc = get_prev_date(db, prev_date);
  if (rc){
    fprintf(stderr, "Error gettin previous day date\n");
    return NULL;
  }
  time_t t = time(NULL);
  strftime(date_today, sizeof(date_today), "%Y-%m-%d", localtime(&t));
  if (strcmp(date_today, prev_date) == 0){
    return NULL;
  } else {
    return prev_date;
  }
}

void get_daily_log_eod(int arr[], char *notes) {
  printf("Eat out meals (0-3): ");
  scanf("%d", &arr[0]);
  printf("\n");

  printf("Home cooked? (1 = yes, 0 = no): ");
  scanf("%d", &arr[1]);
  printf("\n");

  printf("Steps: ");
  scanf("%d", &arr[2]);
  printf("\n");

  printf("Outside time (mins): ");
  scanf("%d", &arr[3]);
  printf("\n");

  printf("Screen time (mins): ");
  scanf("%d", &arr[4]);
  printf("\n");

  printf("Exercise? (1 = yes, 0 = no): ");
  scanf("%d", &arr[5]);
  printf("\n");

  printf("Caffeine drinks: ");
  scanf("%d", &arr[6]);
  printf("\n");

  printf("Stress (1-5): ");
  scanf("%d", &arr[7]);
  printf("\n");

  printf("Productive feel (1-5): ");
  scanf("%d", &arr[8]);
  printf("\n");

  getchar();
  printf("Notes: ");
  fgets(notes, 50, stdin);
  notes[strcspn(notes, "\n")] = '\0';
  printf("\n");
}

void get_expense_input(int arr[], char *notes) {
    const char *categories[] = {
        "Food", "Transport", "Groceries", "Utilities", "Rent",
        "Health", "Education", "Entertainment", "Clothing", "Other"
    };
    const char *recurrence_str[] = { "One-off", "Weekly", "Fortnightly", "Monthly" };
    const char *payment_str[]    = { "Cash", "Card", "Tap" };

    printf("| %-3s | %-15s |\n", "ID", "CATEGORY");
    printf("|-----|------------------|\n");
    for (int i = 0; i < 10; i++)
        printf("| %-3d | %-15s |\n", i, categories[i]);
    printf("Enter category: ");
    scanf("%d", &arr[0]);
    printf("\n");

    double amount;
    printf("Enter amount (e.g. 15.00): ");
    scanf("%lf", &amount);
    arr[1] = (int)round(amount * 100);
    printf("\n");

    printf("Enter need score (1-5): ");
    scanf("%d", &arr[2]);
    printf("\n");

    printf("Enter want score (1-5): ");
    scanf("%d", &arr[3]);
    printf("\n");

    double importance = ((arr[2] * needScore) + (arr[3] * wantScore)) / 5;
    arr[4] = (int)round(importance * 100);

    printf("| %-3s | %-10s |\n", "ID", "RECURRENCE");
    printf("|-----|------------|\n");
    for (int i = 0; i < 4; i++)
        printf("| %-3d | %-10s |\n", i, recurrence_str[i]);
    printf("Enter recurrence: ");
    scanf("%d", &arr[5]);
    printf("\n");

    printf("Planned? (1 = yes, 0 = no): ");
    scanf("%d", &arr[6]);
    printf("\n");

    printf("| %-3s | %-10s |\n", "ID", "PAYMENT");
    printf("|-----|------------|\n");
    for (int i = 0; i < 3; i++)
        printf("| %-3d | %-10s |\n", i, payment_str[i]);
    printf("Enter payment method: ");
    scanf("%d", &arr[7]);
    printf("\n");

    arr[8] = (int)time(NULL);  // time_of_purchase

    getchar();
    printf("Notes: ");
    fgets(notes, 500, stdin);
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
  int daily_arr[9]; int exp_arr[9]; char log_notes[50]; char exp_notes[50]; int rc;

  printf("Enter previous days daily log and expenses\n");
  get_daily_log_eod(daily_arr, log_notes);
  sql_command_exec(db, "daily_log", update_eod_log, daily_arr, 9, log_notes, date);
  int count = 4;
  ExpenseEntry *entry = malloc(count * sizeof(ExpenseEntry));
  rc = get_expense_loop(&entry, &count);
  if (rc) return 1;
  rc = expense_entry(db, entry, count, date);
  if (rc) return 1;
  rc = bank_snapshot(db, date);
  if (rc) return 1;

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
  printf("Enter time you went to bed (24hr format): ");
  scanf("%d", &arr[0]);
  printf("\n");
  printf("Enter time you woke up (24hr format): ");
  scanf("%d", &arr[1]);
  printf("\n");
  printf("Enter sleep quality (1 to 5): ");
  scanf("%d", &arr[3]);
  printf("\n");
  printf("Enter mood (1 to 5): ");
  scanf("%d", &arr[4]);
  printf("\n");
  printf("Enter energy (1 to 5): ");
  scanf("%d", &arr[5]);
  printf("\n");
  printf("Enter your bank balance: ");
  scanf("%lf", &amount);
  printf("\n");
  printf("Enter how many subjects do you want: ");
  scanf("%d", &arr[7]);
  getchar();
  printf("\n");
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
  arr[6] = (int)round(amount * 100);
  first_write_into_db(db, arr, sub_arr);
  for (int i = 0; i < arr[7]; i++) free(sub_arr[i]);
  free(sub_arr);
  return 0;
}


void free_memory(sqlite3* db, Subjects* subjects, Task* task){
  sqlite3_close(db);
  free(subjects);
  free(task);
}
