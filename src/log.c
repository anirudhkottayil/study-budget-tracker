#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"
#include "db.h"

// int first_start(){
//   // Get date of previous day daily log
// }

int check_new_day(sqlite3* db){
  char prev_date[11] = {0}; char date_today[11];
  int rc = get_prev_date(db, prev_date);
  if (rc){
    fprintf(stderr, "Error gettin previous day date\n");
    return -1;
  }
  time_t t = time(NULL);
  strftime(date_today, sizeof(date_today), "%Y-%m-%d", localtime(&t));
  if (strcmp(date_today, prev_date) == 0){
    return 0;
  } else {
    return 1;
  }
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
  printf("Enter time you went to bed: ");
  scanf("%d", &arr[0]);
  printf("Enter time you woke up: ");
  scanf("%d", &arr[1]);
  printf("Enter sleep quality (1 to 5): ");
  scanf("%d", &arr[3]);
  printf("Enter mood (1 to 5): ");
  scanf("%d", &arr[4]);
  printf("Enter energy (1 to 5): ");
  scanf("%d", &arr[5]);
  printf("Enter your bank balance: ");
  scanf("%lf", &amount);
  printf("Enter how many subjects do you want: ");
  scanf("%d", &arr[7]);
  getchar();
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
  for (int i = 0; i < arr[6]; i++) free(sub_arr[i]);
  free(sub_arr);
  return 0;
}


void free_memory(sqlite3* db, Subjects* subjects, Task* task){
  sqlite3_close(db);
  free(subjects);
  free(task);
}
