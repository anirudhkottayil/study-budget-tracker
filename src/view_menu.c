#include <stdio.h>
#include "db.h"
void view_subjects(Subjects** subjects, int* num_subjects, int* in_study){
  // change in tasks file for add tasks or update tasks 
  // use sl no
  if (*num_subjects == 0){
    return;
  }
  if (*in_study == 1){
    printf("IN STUDY SESH\n");
  }

  printf("| %-4s | %-3s | %-20s |\n","No:", "ID", "SUBJECT");
  printf("|-----|----------------------|\n");
  for (int i = 0; i < *num_subjects; i++){
      printf("| %-4d | %-3d | %-20s |\n", i + 1, (*subjects)[i].id, (*subjects)[i].subject);
  }
  return; 
}

void view_tasks(Task** tasks, int* num_tasks, int* in_study, Subjects** subjects, int* num_subjects){
  if (*in_study){
    printf("IN STUDY SESH\n");
  }
  if (*num_tasks == 0){
    printf("No unfinished tasks to display\n");
    return;
  }
  printf("| %-4s | %-3s | %-20s | %-15s | %-8s | %-8s |\n",
       "NO:",  "ID", "TASK", "SUBJECT", "EST(min)", "OBS(min)");
  printf("|-----|----------------------|-----------------|----------|----------|\n");

  for (int i = 0; i < *num_tasks; i++) {
      // find subject name
      const char *subj_name = "None";
      for (int j = 0; j < *num_subjects; j++) {
          if ((*subjects)[j].id == (*tasks)[i].subject) {
              subj_name = (*subjects)[j].subject;
              break;
          }
      }
      printf("| %-4d | %-3d | %-20s | %-15s | %-8d | %-8d |\n",
             i + 1,
             (*tasks)[i].id,
             (*tasks)[i].task,
             subj_name,
             (*tasks)[i].estimated_mins,
             (*tasks)[i].observed_mins);
  }

  return;
}
