#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db.h"
#include "sql_commands.h"
#include "constants.h"
#include "utils.h"
#include "view_menu.h"

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
  user_input = read_int_input("", 1, 7);
  return user_input;
}

int get_study_info(int* arr,Subjects** subjects, int* sub_num, char notes[]){
  arr[3] = read_int_input("How was the mood before the sesh (1 to 5)",1,5);
  arr[4] = read_int_input("How was the mood after the sesh (1 to 5)",1,5);
  arr[5] = read_int_input("How was the energy before the sesh (1 to 5)",1,5);
  arr[6] = read_int_input("How was the focus during the sesh (1 to 5)",1,5);
  view_subjects(subjects, sub_num, NULL);
  int sub_index = read_int_input("Which subject did you study (No:)", 1, *sub_num) - 1;
 
  arr[7] = (*subjects)[sub_index].id;
  int env = print_environment();
  arr[8] = read_int_input("Enter the ID of the environment", 0, env);
  printf("Enter any notes you have on the session\n");
  fgets(notes, 50, stdin);
  notes[strcspn(notes, "\n")] = '\0';
  return 0;
  
}

int get_study_task(sqlite3* db, Task** tasks, int* task_num, int duration, Subjects** subjects, int* num_subjects){
  int arr[3]; int rc; int fin_task;
  if (*task_num == 0){
    return 0;
  }
  view_tasks(tasks, task_num, NULL, subjects, num_subjects);
  arr[1] = read_int_input("Enter the (No:) of the task: ", 1, *task_num) - 1;
  arr[0] = (int)duration/60;
  
  int adx = -1;
  for (int i = 0; i < *task_num; i++){
    if ((*tasks)[i].id == arr[1]){
      adx = i;
      break;
    }
  }

  if (adx == -1) return 1;

  rc = sql_command_exec(db, update_task_time, arr, 2, NULL, NULL);

  if (rc) return -1;
  (*tasks)[adx].observed_mins += duration;

  fin_task = read_int_input("Did you finish the task ? (1 if yes)", 0, 1);

  if (fin_task){
    rc = sql_command_exec(db, complete_task, &arr[1], 1, NULL, NULL);
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
  int insert_result = sql_command_exec(db, insert_study_session, arr, 10, notes, NULL);
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
    user_input = read_int_input("Enter your choice: ", 1, 4);

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
      rc = get_study_task(db, task,num_tasks, *study_stop - *study_start, subjects, sub_num);
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
