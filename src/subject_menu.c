#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "subject_menu.h"
#include "db.h"
#include "sql_commands.h"

int add_subject(sqlite3* db, Subjects** subjects, int* num_subjects, char* name){

  sql_command_exec(db,"subjects", insert_subjects, NULL, 0, name, NULL);
  Subjects* temp_ptr = realloc(*subjects, sizeof(Subjects) * (*num_subjects + 1));
  if (temp_ptr == NULL){
    fprintf(stderr, "Realloc subjects failed\n");
    return 1;
  }
  *subjects = temp_ptr;
  *num_subjects = *num_subjects + 1;
  (*subjects)[*num_subjects - 1].id = (int)sqlite3_last_insert_rowid(db);
  strncpy((*subjects)[*num_subjects].subject, name, 49);
  
  return 0;
}

int view_subjects(Subjects** subjects, int* num_subjects){

  printf("| %-3s | %-20s |\n", "ID", "SUBJECT");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *num_subjects; i++){
      printf("| %-3d | %-20s |\n", (*subjects)[i].id, (*subjects)[i].subject);

  return 0;
}


int subjects_menu(sqlite3*db, int* in_study, Subjects** subjects, int* num_subjects){
  int user_input ; int rc; int if_input;
  while (1){
    if (*in_study){
      printf("__SUBJECTS_MENU___ (IN STUDY SESH)\n");
    } else {
      printf("___SUBJECTS_MENU___\n");
    }
    printf("Enter 1 to view subjects\n");
    printf("Enter 2 to add a subject\n");
    printf("Enter 3 to delete a subject\n");
    printf("Enter 4 to return to main menu\n");
    scanf("%d", &user_input);
    getchar();

    if (user_input == 1){
      rc = view_subjects(subjects, num_subjects);
      if (rc) {
        return 1;
      }
    } else if (user_input == 2){
      char subject[50];
      printf("Enter the name of the subject: ");
      scanf("%49s", subject);
      getchar();
      subject[49] = '\0';
      printf("\n");
      rc = add_subject(db, subjects, num_subjects, subject);
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
