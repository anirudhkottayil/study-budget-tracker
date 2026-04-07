#include <stdio.h>
#include "db.h"
#include "log.h"

int main(){
  sqlite3 *db;
  int rc = initialize_db(&db);
  if (rc){return 0;}

  int is_first = check_first_time_run();
  if (is_first){
    int bed_time, wakeup, sleep_quality, mood, energy;
    printf("Enter time you went to bed: ");
    scanf("%d\n", &bed_time);
    printf("Enter time you woke up: ");
    scanf("%d\n", &wakeup);
    printf("Enter sleep quality (1 to 5): ");
    scanf("%d\n", &sleep_quality);
    printf("Enter mood (1 to 5): ");
    scanf("%d\n", &mood);
    printf("Enter energy (1 to 5): ");
    scanf("%d\n", &energy);
    
    
  }
  return 0;
}
