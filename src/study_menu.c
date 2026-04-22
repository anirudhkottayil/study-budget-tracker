#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "sql_commands.h"
#include <time.h>

int main_menu(int* in_study){
  int user_input;
  if (*in_study){
    printf("___MAIN MENU___ (IN STUDY SESH)\n");
  } else {
    printf("___MAIN MENU___\n");
  }
  printf("Enter 1 for Study menu\n");
  printf("Enter 2 for Daily Log menu\n");
  printf("Enter 3 for Expenses menu\n");
  printf("Enter 4 for Subjects menu\n");
  printf("Enter 5 for income menu\n");
  printf("Enter 6 for Tasks menu\n");
  printf("Enter 7 to exit\n");
  scanf("%d", &user_input);
  getchar();



  return user_input;
}

int get_study_info(int* arr,Subjects** subjects, int* sub_num, char notes[]){
  const char* environments[] = {"Desk", "Bed", "Couch", "Kitchen Table", "Work Table"};
  int env_count = 5;
  printf("How was the mood before the sesh (1 to 5)\n");
  scanf("%d",&arr[3]);
  printf("How was the mood after the sesh (1 to 5)\n");
  scanf("%d",&arr[4]);
  printf("How was the energy before the sesh (1 to 5)\n");
  scanf("%d",&arr[5]);
  printf("How was the focus during the sesh (1 to 5)\n");
  scanf("%d",&arr[6]);
  printf("Which subject did you study\n");
  printf("| %-3s | %-20s |\n", "ID", "SUBJECT");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *sub_num; i++){
      printf("| %-3d | %-20s |\n", (*subjects)[i].id, (*subjects)[i].subject);
  }
  scanf("%d",&arr[7]);
  printf("| %-3s | %-15s |\n", "ID", "ENVIRONMENT");
  printf("|-----|-----------------|\n");
  for (int i = 0; i < env_count; i++){
      printf("| %-3d | %-15s |\n", i, environments[i]);
  }
  printf("|-----|-----------------|\n");
  scanf("%d",&arr[8]);
  getchar();

  printf("Enter any notes you have on the session\n");
  fgets(notes, 50, stdin);
  notes[strcspn(notes, "\n")] = '\0';

  return 0;
  
}

int get_study_task(sqlite3* db, Task** tasks, int* task_num, int duration){
  int arr[3]; int rc; int fin_task;
  if (*task_num == 0){
    return 0;
  }
  printf("Which task did you work on\n");
  printf("| %-3s | %-20s |\n", "ID", "TASK");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *task_num; i++){
      printf("| %-3d | %-20s |\n",(*tasks)[i].id,(*tasks)[i].task);
  }
  scanf("%d",&arr[1]);
  arr[0] = (int)duration/60;
  getchar();

  int adx = -1;
  for (int i = 0; i < *task_num; i++){
    if ((*tasks)[i].id == arr[1]){
      adx = i;
      break;
    }
  }

  if (adx == -1) return 1;

  rc = sql_command_exec(db, "tasks", update_task_time, arr, 2, NULL, NULL);

  if (rc) return -1;
  (*tasks)[adx].observed_mins += duration;

  printf("Did you finish the task ? (1 if yes)\n");
  scanf("%d",&fin_task);

  if (fin_task){
    rc = sql_command_exec(db, "tasks", complete_task, &arr[1], 1, NULL, NULL);
    if (rc) return -1;
    for (; adx < *task_num; adx++){
      if (adx == (*task_num-1)) break;
      (*tasks)[adx] = (*tasks)[adx+1];
    }

    (*task_num)--;
    if (*task_num == 0){
      free(*tasks);
      return 0;
    }
    Task* tmp = realloc(*tasks, *task_num * sizeof(Task));
    if (tmp == NULL){
      fprintf(stderr, "Error resizing struct task on completion in study session\n");
      return -1;
    }
    *tasks = tmp;
  }

  return 0;
}

int post_study_session(sqlite3*db, int* study_start, int* study_stop, int* distraction_count, Subjects** subjects, int* sub_num) {
  int arr[10];
  arr[0] = *study_start;
  arr[1] = *study_stop;
  arr[2] = (*study_stop - *study_start) / 60;
  arr[9] = *distraction_count;
  char notes[50];
  int rc = get_study_info(arr, subjects, sub_num, notes);
  if (rc){
    return 1;
  }
  int insert_result = sql_command_exec(db, "study_sessions", insert_study_session, arr, 10, notes, NULL);
  if (insert_result){
    fprintf(stderr, "Failed to insert into study sessions");
    return 1;
  }

  return 0;
}

int study_menu(sqlite3* db, int* in_study, int* study_start, int* study_stop, int* distraction_count, Subjects** subjects, int* sub_num, Task** task, int* num_tasks){
  int user_input = 0;
  time_t start, stop;
  while (1){
    if (*in_study){
      printf("___STUDY MENU___ (IN STUDY SESH)\n");
    } else {
      printf("___STUDY MENU___\n");
    }
    if (*in_study != 1){
      printf("Enter 1 to  Start study sesh\n");
    } else {
      printf("Enter 2 to Stop study sesh\n");
    }
    printf("Enter 3 to go back to menu\n");
    printf("Enter 4 to increment distracted count\n");
    scanf("%d", &user_input);
    getchar();

    if (user_input == 1 && *in_study == 0){
      start = time(NULL);
      *study_start = (int)start;
      *in_study = 1;
      *distraction_count = 0;
    } else if (user_input == 2 && *in_study == 1){
      stop = time(NULL);
      *study_stop = (int)stop;
      *in_study = 0;
      int rc = post_study_session(db, study_start, study_stop, distraction_count, subjects, sub_num); 
      if (rc){
        fprintf(stderr, "Study session insert failed");
        return 1;
      }
      rc = get_study_task(db, task,num_tasks, *study_stop - *study_start);
      *distraction_count = 0;
    } else if (user_input == 4 && *in_study == 1){
      (*distraction_count)++;
    } else if (user_input == 3){
      return 0;
    } else {
      printf("Invalid input try again");
    }
  }
  return 0;
}
