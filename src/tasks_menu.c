#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tasks_menu.h"
#include "sql_commands.h"

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

void view_tasks(Task** tasks, int* num_tasks, int* in_study){
  if (*in_study){
    printf("IN STUDY SESH\n");
  }

  printf("| %-3s | %-20s |\n", "ID", "TASK");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *num_tasks; i++){
      printf("| %-3d | %-20s |\n", (*tasks)[i].id, (*tasks)[i].task);

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
    printf("Enter 3 to mark a task complete\n");
    printf("Enter 4 to return to main menu\n");
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
      char subject[50];
      printf("Enter the name of the subject you want to add: ");
      scanf("%49s", subject);
      getchar();
      subject[49] = '\0';
      printf("\n");
      rc = delete_subject(db, subjects, num_subjects, subject);
      if (rc == 1) {
        return 1;
      } else if (rc == -1) {
        printf("Subject not found\n");
      } else {
        printf("Success\n");
      }

    } else if (user_input == 4){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }

  return 0;
}
