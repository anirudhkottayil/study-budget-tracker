#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "daily_log_menu.h"
#include "db.h"
#include "sql_commands.h"
#include "mapper_func.h"

void print_logs(Daily_logs* log, int num_logs, int* in_study){
  if (*in_study){
    printf("In Study Session\n");
  }
  for (int i = 0; i < num_logs; i++) {
    char sleep_buf[32], wake_buf[32];
    time_t t;

    t = log[i].sleep_time;
    strftime(sleep_buf, sizeof(sleep_buf), "%H:%M", localtime(&t));

    t = log[i].wake_time;
    strftime(wake_buf, sizeof(wake_buf), "%H:%M", localtime(&t));

    printf("┌─────────────────────────────────────┐\n");
    printf("│  Date:            %s\n",     log[i].date);
    printf("│  Sleep Time:      %s\n",     sleep_buf);
    printf("│  Wake Time:       %s\n",     wake_buf);
    printf("│  Sleep Duration:  %d min\n", log[i].sleep_dur_min);
    printf("│  Sleep Quality:   %d/5\n",   log[i].sleep_quality);
    printf("│  Mood:            %d/5\n",   log[i].mood);
    printf("│  Energy:          %d/5\n",   log[i].energy);
    printf("│  Eat Out Meals:   %d\n",     log[i].eat_out_meals);
    printf("│  Home Cooked:     %s\n",     log[i].home_cooked ? "Yes" : "No");
    printf("│  Steps:           %d\n",     log[i].steps);
    printf("│  Outside:         %d min\n", log[i].outside_min);
    printf("│  Screen Time:     %d min\n", log[i].screen_time_min);
    printf("│  Exercise:        %s\n",     log[i].exercise ? "Yes" : "No");
    printf("│  Caffeine:        %d drinks\n", log[i].caffeine_drinks);
    printf("│  Stress:          %d/5\n",   log[i].stress);
    printf("│  Productive Feel: %d/5\n",   log[i].productive_feel);
    printf("│  Notes:           %s\n",     log[i].notes[0] ? log[i].notes : "—");
    printf("└─────────────────────────────────────┘\n");
  }
}

int view_logs(sqlite3* db, int input, int* in_study){

  int arr[1];
  int db_rows = count_rows(db, count_daily_logs);
  if ( db_rows == -1){
    fprintf(stderr, "Error getting row count for daily logs\n");
    return 1;
  }
  arr[0]= (input > db_rows) ? db_rows : input;

  if (arr[0] == 0) return 0;
  Daily_logs* logs = malloc(arr[0] * sizeof(Daily_logs));
  int rc = get_rows(db, get_n_logs, logs, map_logs, arr, 1) ;
  if (rc == -1){
    fprintf(stderr, "Error getting rows for daily logs\n");
    return 1;
  }

  print_logs(logs, arr[0], in_study);
  free(logs);
  return 0;
}

int update_log(sqlite3* db, char* date, int* in_study){
  int arr[15]; int rc;
  printf("Enter sleep time: ");
  scanf("%d", &arr[0]);
  printf("\n");
  printf("Enter updated wake time: ");
  scanf("%d", &arr[1]);
  printf("\n");
  printf("Enter sleep duration: ");
  scanf("%d", &arr[2]);
  printf("\n");
  printf("Enter sleep quality: ");
  scanf("%d", &arr[3]);
  printf("\n");
  printf("Enter mood: ");
  scanf("%d", &arr[4]);
  printf("\n");
  printf("Enter energy: ");
  scanf("%d", &arr[5]);
  printf("\n");
  printf("Enter eat out meals: ");
  scanf("%d", &arr[6]);
  printf("\n");
  printf("Enter home cooked meals: ");
  scanf("%d", &arr[7]);
  printf("\n");
  printf("Enter steps: ");
  scanf("%d", &arr[8]);
  printf("\n");
  printf("Enter outside mins: ");
  scanf("%d", &arr[9]);
  printf("\n");
  printf("Enter screen time mins: ");
  scanf("%d", &arr[10]);
  printf("\n");
  printf("Enter exercise: ");
  scanf("%d", &arr[11]);
  printf("\n");
  printf("Enter caffeine drinks: ");
  scanf("%d", &arr[12]);
  printf("\n");
  printf("Enter stress: ");
  scanf("%d", &arr[13]);
  printf("\n");
  printf("Enter productive feel: ");
  scanf("%d", &arr[14]);
  printf("\n");
  getchar();
  char temp_notes[50];
  fgets(temp_notes, 50, stdin);
  temp_notes[strcspn(temp_notes, "\n")] = '\0'; 

  rc = sql_command_exec(db, "daily_log", update_daily_log, arr, 15, temp_notes, date);
  if (rc){
    fprintf(stderr, "Error updating the log\n");
    return 1;
  }
  
  return 0;
}

int daily_log_menu(sqlite3* db, int* in_study){
  int user_input ; int rc; int if_input;
  while (1){
    if (*in_study){
      printf("__DAILY_LOG_MENU___ (IN STUDY SESH)\n");
    } else {
      printf("___DAILY_LOG_MENU___\n");
    }
    printf("Enter 1 to view logs\n");
    printf("Enter 2 to update logs\n");
    printf("Enter 3 to go back to main menu\n");
    scanf("%d", &user_input);
    getchar();

    if (user_input == 1){
      printf("How many previous days logs do you want to see : ");
      scanf("%d", &if_input);
      getchar();
      printf("\n");
      rc = view_logs(db, if_input, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 2){
      char date[11];
      printf("Enter date of the log you want to update (YYYY-MM-DD): ");
      scanf("%10s", date);
      getchar();
      date[10] = '\0';
      printf("\n");
      rc = update_log(db, date, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 3){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }
  return 0;

}
