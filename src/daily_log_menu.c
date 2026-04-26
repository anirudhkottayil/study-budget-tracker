#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "daily_log_menu.h"
#include "db.h"
#include "sql_commands.h"
#include "utils.h"
#include "mapper_func.h"

void print_logs(Daily_logs* log, int num_logs, int* in_study){
  if (*in_study){
    printf("In Study Session\n");
  }
  char sleep_buf[32], wake_buf[32];
  int sleep_hour; int sleep_mins; int wake_hours; int wake_mins;
  for (int i = 0; i < num_logs; i++) {
    sleep_hour = log[i].sleep_time / 100;
    sleep_mins = log[i].sleep_time % 100;
    wake_hours = log[i].wake_time / 100;
    wake_mins = log[i].wake_time % 100;
    sprintf(sleep_buf, "%02d:%02d", sleep_hour, sleep_mins);
    sprintf(wake_buf, "%02d:%02d", wake_hours, wake_mins);

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

int count_logs(sqlite3* db){

  int arr[1];
  int db_rows = count_rows(db, count_daily_logs);
  if ( db_rows == -1){
    fprintf(stderr, "Error getting row count for daily logs\n");
    return -1;
  }
  return db_rows;
}

int view_logs(sqlite3* db, int input, int* in_study){

  int arr[1] = {input};
  Daily_logs* logs = malloc(input * sizeof(Daily_logs));
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
  arr[0] = read_int_input("Enter sleep time (24hr format): ", 0000, 2359);
  arr[1] = read_int_input("Enter updated wake time (24hr format): ", 0000, 2359);
  arr[3] = read_int_input("Enter sleep quality: ", 1, 5);
  arr[4] = read_int_input("Enter mood: ", 1, 5);
  arr[5] = read_int_input("Enter energy: ", 1, 5);
  arr[6] = read_int_input("Enter meals you ate out: ", 0, 3);
  arr[7] = read_int_input("Enter home cooked meals: ", 0, 1);
  arr[8] = read_int_input("Enter steps: ", 0, 65000);
  arr[9] = read_int_input("Enter outside mins: ", 0, 1080);
  arr[10] = read_int_input("Enter screen time mins: ", 1, 1080);
  arr[11] = read_int_input("Enter exercise:", 0, 1);
  arr[12] = read_int_input("Enter caffeine drinks: ", 0, 5);
  arr[13] = read_int_input("Enter stress: ", 0, 5);
  arr[14] = read_int_input("Enter productive feel: ", 1, 5);
  char temp_notes[50];
  fgets(temp_notes, 50, stdin);
  temp_notes[strcspn(temp_notes, "\n")] = '\0'; 

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
      int logs = count_logs(db);
      if (logs == -1) return 1;
      int num = read_int_input("How many previous days logs do you want to see : ", 0, logs);
      if (num > 0){
        rc = view_logs(db, num, in_study);
        if (rc) {
          return 1;
        }
      }
    } else if (user_input == 2){
      char* curr_date = read_date_input("Enter date of the log you want to update (YYYY-MM-DD): ");
      rc = update_log(db, curr_date, in_study);
      free(curr_date);
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
