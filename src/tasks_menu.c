#include <stdio.h>
#include "tasks_menu.h"

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
      char subject[50];
      printf("Enter the name of the subject you want to add: ");
      scanf("%49s", subject);
      getchar();
      subject[49] = '\0';
      printf("\n");
      rc = add_subject(db, subjects, num_subjects, subject);
      if (rc == 1) {
        return 1;
      } else if (rc == -1) {
        printf("Subject already in db\n");
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
