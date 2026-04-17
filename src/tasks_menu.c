#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tasks_menu.h"
#include "sql_commands.h"

void view_tasks(Task** tasks, int* num_tasks, int* in_study){
  if (*in_study){
    printf("IN STUDY SESH\n");
  }

  printf("| %-3s | %-20s |\n", "ID", "TASK");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *num_tasks; i++){
      printf("| %-3d | %-20s |\n", (*tasks)[i].id, (*tasks)[i].task);

}

int complete_a_task(sqlite3* db, Task** tasks, int* num_tasks, int* task_id){
  int rc; int fin_task; int* arr[1];
  if (task_id != NULL){
    rc = sql_command_exec(db, "tasks", complete_task, task_id, 1, NULL, NULL);

    if (rc) return -1;
  }
  view_tasks(tasks, num_tasks, in_study);
  printf("Which task did you work on\n");
  scanf("%d", &arr[0]);
  printf("Did you finish the task ? (1 if yes)\n");
  scanf("%d",&fin_task);
  getchar();
  if (*fin_task){
    rc = sql_command_exec(db, "tasks", complete_task, arr, 1, NULL, NULL);
   if (rc) return -1;
  }

  return 0;
}

int update_task(sqlite3* db, Task** tasks, int* num_tasks, int* in_study){
  int arr[3]; int rc; int fin_task;
  printf("Which task did you work on\n");
  printf("| %-3s | %-20s |\n", "ID", "TASK");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *num_tasks; i++){
      printf("| %-3d | %-20s |\n",(*tasks)[i].id,(*tasks)[i].task);
  }
  scanf("%d",&arr[1]);
  printf("Enter time you spent on the task right now (in mins): ");
  scanf("%d", &arr[0]);
  arr[0] = (int) arr[0];
  getchar();

  rc = sql_command_exec(db, "tasks", update_task_time, arr, 2, NULL, NULL);

  if (rc) {
    fprintf(stderr, "Update task time failed\n");
    return -1;
  } 
  printf("Did you finish the task ? (1 if yes)\n");
  scanf("%d",&fin_task);
  getchar();

  if (fin_task){
    arr[0] = 1;
    rc = complete_a_task(db, NULL, NULL, &arr);
  }

  return 0;
}

int add_task(sqlite3* db, Task** tasks, int* num_tasks, char* name){
  int add_idx = -1;
  for (int i = 0; i < *num_tasks; i++){
    if (strcmp((*tasks)[i].task, name) == 0){
        add_idx = i;
        break;
    }
  }

  if (add_idx != -1) return -1;

  int est[1], rc;
  printf("Enter estimated minutes for the task: ");
  scanf("%d", &est[0]);
  getchar();
  printf("\n");

  rc = sql_command_exec(db, "tasks", insert_task, est, 1, name, NULL);
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
  strncpy((*tasks)[*num_tasks - 1].task, name, 49);

  return 0;
}


int tasks_menu(sqlite3* db, int* in_study, Task** tasks, int* num_tasks){

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
      rc = view_tasks(tasks, num_tasks, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 2){
      char task[50];
      printf("Enter the name of the task you want to add: ");
      scanf("%49s", task);
      getchar();
      subject[49] = '\0';
      printf("\n");
      rc = add_task(db, tasks, num_subjects, task);
      if (rc == 1) {
        return 1;
      } else if (rc == -1) {
        printf("Task already in db\n");
      } else {
        printf("Success\n");
      }
    } else if (user_input == 3){
        rc = update_task(db, tasks, num_tasks, in_study);
      if (rc == 1) {
        return 1;
      } else if (rc == -1) {
        printf("Task not found\n");
      } else {
        printf("Success\n");
      }

    } else if (user_input == 4){
        rc = complete_a_task(db, tasks, num_tasks, in_study);
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
