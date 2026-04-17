#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "subject_menu.h"
#include "db.h"
#include "sql_commands.h"

int delete_subject(sqlite3* db, Subjects** subjects, int* num_subjects, char* name){

  int del_idx = -1;
  for (int i = 0; i < *num_subjects; i++){
      if (strcmp((*subjects)[i].subject, name) == 0){
          del_idx = i;
          break;
      }
  }

  if (del_idx == -1) return -1;  

  for (int i = del_idx; i < *num_subjects - 1; i++){
      (*subjects)[i] = (*subjects)[i + 1];
  }

  sql_command_exec(db,"subjects", delete_subject_sql, NULL, 0, name, NULL);
  Subjects* temp_ptr = realloc(*subjects, sizeof(Subjects) * (*num_subjects - 1));
  if (temp_ptr == NULL){
    fprintf(stderr, "Realloc subjects failed\n");
    return 1;
  }
  *subjects = temp_ptr;
  *num_subjects = *num_subjects - 1;
  return 0;
}

int add_subject(sqlite3* db, Subjects** subjects, int* num_subjects, char* name){

  int add_idx = -1;
  for (int i = 0; i < *num_subjects; i++){
      if (strcmp((*subjects)[i].subject, name) == 0){
          add_idx = i;
          break;
      }
  }

  if (add_idx != -1) return -1;

  sql_command_exec(db,"subjects", insert_subjects, NULL, 0, name, NULL);
  Subjects* temp_ptr = realloc(*subjects, sizeof(Subjects) * (*num_subjects + 1));
  if (temp_ptr == NULL){
    fprintf(stderr, "Realloc subjects failed\n");
    return 1;
  }
  *subjects = temp_ptr;
  *num_subjects = *num_subjects + 1;
  (*subjects)[*num_subjects - 1].id = (int)sqlite3_last_insert_rowid(db);
  strncpy((*subjects)[*num_subjects - 1].subject, name, 49);
  
  return 0;
}

int view_subjects(Subjects** subjects, int* num_subjects, int* in_study){
  if (*in_study){
    printf("IN STUDY SESH\n");
  }

  printf("| %-3s | %-20s |\n", "ID", "SUBJECT");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *num_subjects; i++){
      printf("| %-3d | %-20s |\n", (*subjects)[i].id, (*subjects)[i].subject);

  return 0;
}


int subjects_menu(sqlite3*db, int* in_study, Subjects** subjects, int* num_subjects){
  int user_input ; int rc; 
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
      rc = view_subjects(subjects, num_subjects, in_study);
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
      printf("Enter the name of the subject you want to delete: ");
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
