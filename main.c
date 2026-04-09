#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "db.h"
#include "log.h"

int main(){
  sqlite3 *db;
  int rc = initialize_db(&db);
  if (rc){return 0;}

  int is_first = check_first_time_run();
  if (is_first){

     
  }
  return 0;
}
