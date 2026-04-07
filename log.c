#include <stdio.h>
#include <time.h>
#include "log.h"
#include "db.h"

int first_start(){
  // Get date of previous day daily log
}

int check_first_time_run(){
  FILE *file = fopen(".appran", "r");
  if (file){
    fclose(file);
    return 1;
  } else{
    file = fopen(".appran", "w");
    return 0;
  }
}
