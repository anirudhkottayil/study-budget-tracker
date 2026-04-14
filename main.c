#include <stdio.h>
#include "mapper_func.h"
#include <string.h>
#include <stdlib.h>
#include "db.h"
#include "log.h"
#include "menu.h"
#include "sql_commands.h"

int main(){
  sqlite3 *db;
  int in_study = 0, study_start = 0, study_stop = 0, distraction_count = 0;
  int rc, user_input, menu_check;
  int loop = -1;
  rc = initialize_db(&db);
  if (rc){return 0;}

  int is_first = check_first_time_run();
  if (is_first){
    rc = first_run(db);
    if (rc) { return 1; }
    rc = first_time_data(db); 
    if (rc) { return 1; }
  }
  
  int num_subjects = count_rows(db, count_subjects);
  Subjects* subjects = malloc(num_subjects * sizeof(Subjects));
  rc = get_rows(db, get_subjects , (void*) subjects, map_subject);
  if (rc){
    sqlite3_close(db);
    free(subjects);
  }

  int num_tasks = count_rows(db, count_tasks);
  Task* tasks = malloc(num_tasks * sizeof(Task));
  rc = get_rows(db,get_incomplete_tasks, (void*) tasks, map_tasks);
  if (rc){
    free_memory(db, subjects, tasks);
  }

  while (loop == -1){
    user_input = main_menu(&in_study);

    if (user_input == 1) {
      menu_check = study_menu(db, &in_study, &study_start,
                              &study_stop, &distraction_count, &subjects, &num_subjects, &tasks, &num_tasks);
      if (menu_check){
        loop = 0;
      }
    } else if (user_input == 2){

    } else if (user_input == 3){

    } else if (user_input == 4){

    } else if (user_input == 5){

    } else if (user_input == 6){
      printf("Bye Bye\n");
      loop = 0;
    } else {
      printf("Invalid input! Try again\n");
    }
  }

  free_memory(db, subjects, tasks);
  return 0;
}
