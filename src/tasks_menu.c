#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tasks_menu.h"
#include "utils.h"
#include "sql_commands.h"
#include "subject_menu.h"
#include "view_menu.h"


int complete_a_task(sqlite3* db, Task** tasks, int* num_tasks,int* in_study, int* task_id, Subjects** subjects, int* num_subjects){
  int rc; int fin_task; int arr[1];
  if (task_id != NULL){
    rc = sql_command_exec(db, "tasks", complete_task, task_id, 1, NULL, NULL);
    if (rc) return -1;
    for (int i = 0; i < *num_tasks; i++){
      if ((*tasks)[i].id == *task_id){
        for (int j = i; j < *num_tasks; j++){
          if (j != *num_tasks -1){
           (*tasks)[j] = (*tasks)[j+1];
          }
        }
        break;
      }
    }
    (*num_tasks)--;
    Task* tmp = realloc(*tasks, *num_tasks * sizeof(Task));
    if (tmp == NULL){
      fprintf(stderr, "Realloc task failed\n");
      return 1;
    }
    *tasks = tmp;
    return 0; 
  }
  view_tasks(tasks, num_tasks, in_study, subjects, num_subjects);
  int index = read_int_input("Enter No: of task did you work on", 1, *num_tasks);
  arr[0] = (*tasks)[index-1].id;
  fin_task = read_int_input("Did you finish the task ? (1 if yes)", 0, 1);
  if (fin_task){
    rc = sql_command_exec(db, "tasks", complete_task, arr, 1, NULL, NULL);
   if (rc) return -1;
    for (int i = 0; i < *num_tasks; i++){
      if ((*tasks)[i].id == arr[0]){
        for (int j = i; j < *num_tasks; j++){
          if (j != *num_tasks -1){
           (*tasks)[j] = (*tasks)[j+1];
          }
        }
        break;
      }
    }
    (*num_tasks)--;
    Task* tmp = realloc(*tasks, *num_tasks * sizeof(Task));
    if (tmp == NULL){
      fprintf(stderr, "Realloc task failed\n");
      return 1;
    }
    *tasks = tmp;
  }

  return 0;
}

int update_task(sqlite3* db, Task** tasks, int* num_tasks, int* in_study,Subjects** subjects, int* num_subjects){
  int arr[3]; int rc; int fin_task;
  if (*num_tasks == 0){
    printf("No unfinished tasks to display.\n");
    return 0;
  }
  view_tasks(tasks, num_tasks, in_study, subjects, num_subjects);
  printf("Which No: task did you work on\n");
  int index = read_int_input("Which No: task did you work on", 1, *num_tasks);
  arr[1] = (*tasks)[index-1].id;
  printf("Enter time you spent on the task right now (in mins): ");
  scanf("%d", &arr[0]);
  getchar();

  rc = sql_command_exec(db, "tasks", update_task_time, arr, 2, NULL, NULL);

  if (rc) {
    fprintf(stderr, "Update task time failed\n");
    return -1;
  } 
  for (int i = 0; i < *num_tasks; i++){
    if ((*tasks)[i].id == arr[1]){
      (*tasks)[i].observed_mins += arr[0];
      break;
    }
  }
  fin_task = read_int_input("Did you finish the task ? (1 if yes)", 0, 1);

  if (fin_task){
    rc = complete_a_task(db, tasks, num_tasks, in_study, &arr[1], NULL, NULL);
  }

  return 0;
}

int add_task(sqlite3* db, Task** tasks, int* num_tasks, char* name, Subjects** subjects, int* num_subjects, int* in_study){
  int add_idx = -1;
  if (*num_tasks > 0){
    for (int i = 0; i < *num_tasks; i++){
      if (strcmp((*tasks)[i].task, name) == 0){
          add_idx = i;
          break;
      }
    }
    if (add_idx != -1) return -1;
  }


  int est[2], rc;
  est[0] = 0;
  printf("Enter estimated minutes for the task: ");
  scanf("%d", &est[0]);
  getchar();
  printf("\n");
  
  view_subjects(subjects, num_subjects, in_study);
  printf("| %-3s | %-20s |\n", "ID", "SUBJECT");
  printf("|-----|----------------------|\n");
  printf("| %-3d | %-20s |\n", 0, "None");
  for (int i = 0; i < *num_subjects; i++)
      printf("| %-3d | %-20s |\n", (*subjects)[i].id, (*subjects)[i].subject);
  int index = read_int_input("Enter subject ID (-1 for none): ", 1, *num_subjects);
  est[1] = (*subjects)[index-1].id;
  rc = sql_command_exec(db, "tasks", insert_task, est, 2, name, NULL);
  if (rc){
    fprintf(stderr, "Insert task failed\n");
    return 1;
  }

  Task* temp_ptr = realloc(*tasks, sizeof(Task) * (*num_tasks + 1));
  if (temp_ptr == NULL){
    fprintf(stderr, "Realloc task failed\n");
    return 1;
  }
  *tasks = temp_ptr;
  *num_tasks = *num_tasks + 1;
  (*tasks)[*num_tasks - 1].id = (int)sqlite3_last_insert_rowid(db);
  (*tasks)[*num_tasks - 1].subject       = est[1];
  strncpy((*tasks)[*num_tasks - 1].task, name, 49);
  (*tasks)[*num_tasks - 1].estimated_mins = est[0];
  (*tasks)[*num_tasks - 1].observed_mins  = 0;
  (*tasks)[*num_tasks - 1].task[49] = '\0';
  return 0;
}


int tasks_menu(sqlite3* db, int* in_study, Task** tasks, int* num_tasks, Subjects** subjects, int* num_subjects){

  int user_input ; int rc; 
  while (1){
    if (*in_study){
      printf("__TASKS_MENU___ (IN STUDY SESH)\n");
    } else {
      printf("___TASKS_MENU___\n");
    }
    printf("Enter 1 to view tasks\n");
    printf("Enter 2 to add a task\n");
    printf("Enter 3 to update time in a task\n");
    printf("Enter 4 to mark a task complete\n");
    printf("Enter 5 to return to main menu\n");
    scanf("%d", &user_input);
    getchar();

    if (user_input == 1){
      view_tasks(tasks, num_tasks, in_study, subjects, num_subjects);

    } else if (user_input == 2){
      char task[50];
      printf("Enter the name of the task you want to add: ");
      scanf("%49s", task);
      getchar();
      task[49] = '\0';
      printf("\n");
      rc = add_task(db, tasks, num_tasks, task, subjects, num_subjects, in_study);
      if (rc == 1) {
        return 1;
      } else if (rc == -1) {
        printf("Task already in db\n");
      } else {
        printf("Success\n");
      }
    } else if (user_input == 3){
        rc = update_task(db, tasks, num_tasks, in_study,subjects, num_subjects);
      if (rc == 1) {
        return 1;
      } else if (rc == -1) {
        printf("Task not found\n");
      } else {
        printf("Success\n");
      }

    } else if (user_input == 4){
        rc = complete_a_task(db, tasks, num_tasks, in_study, NULL, subjects, num_subjects);
      if (rc == 1) {
        printf("Task completion failed\n");
        return 1;
      } else {
        printf("Success\n");
      }

    } else if (user_input == 5){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }

  return 0;
}
