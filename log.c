#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int first_time_data(sqlite3* db){
    int arr[7];
    printf("Enter time you went to bed: ");
    scanf("%d", &arr[0]);
    printf("Enter time you woke up: ");
    scanf("%d", &arr[1]);
    printf("Enter sleep quality (1 to 5): ");
    scanf("%d", &arr[2]);
    printf("Enter mood (1 to 5): ");
    scanf("%d", &arr[3]);
    printf("Enter energy (1 to 5): ");
    scanf("%d", &arr[4]);
    printf("Enter your bank balance: ");
    scanf("%d", &arr[5]);
    printf("Enter how many subjects do you want: ");
    scanf("%d", &arr[6]);
    getchar();
    char** sub_arr = malloc (arr[6]* sizeof(char*));
    for (int i = 0; i < arr[6]; i++){
      sub_arr[i] = malloc(50 * sizeof(char));
      fgets( sub_arr[i], 50*sizeof(char), stdin);
      sub_arr[i][strcspn(sub_arr[i], "\n")] = '\0';
    }
    first_write_into_db(db, arr, sub_arr);
}
