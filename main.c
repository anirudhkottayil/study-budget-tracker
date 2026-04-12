#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "db.h"
#include "log.h"
#include "menu.h"

int main(){
  sqlite3 *db;
  int in_study = 0, study_start = 0, study_stop = 0, distraction_count = 0;
  int rc, user_input;
  int loop = -1;
  rc = initialize_db(&db);
  if (rc){return 0;}

  int is_first = check_first_time_run();
  if (is_first){
    rc = first_time_data(db); // Add environment data too
  }
  
  int num_subjects = count_subjects(db);
  Subjects* subjects = malloc(num_subjects * sizeof(Subjects));
  rc = get_subjects(db, subjects);

  while (loop == -1){
    user_input = main_menu(&in_study);

    if (user_input == 1) {
      study_menu(&in_study, &study_start, &study_stop, &distraction_count, subjects, num_subjects);
    }
  }
  return 0;
}
