#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

double read_double_input(const char* prompt, double min, double max){
  double val = 0;
  while (1) {
    printf("%s (%.2f - %.2f): ", prompt, min, max);
    if (scanf("%lf", &val) == 1 && val >= min && val <= max){
      printf("\n");
      getchar();
      break;
    }
    printf("\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("Enter values in the range please\n");
  }

  return val;
}

int read_int_input(const char* prompt, int min, int max){
  int val = 0;
  while (1) {
    printf("%s (%d - %d): ", prompt, min, max);
    if ( scanf("%d", &val) == 1 && val >= min && val <= max){
      printf("\n");
      getchar();
      break;
    }
    printf("\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("Enter values in the range please\n");
  }

  return val;
}

int is_leap_year(int year){
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int check_date_format(Date* date){

  int daysinMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if (date->year <= 0) return 1;

  if (date->month < 1 || date->month > 12) return 1;

  if (date->month == 2 && is_leap_year(date->year)){
    if (date->day < 1 || date->day > 29) return 1;
  } else {
    if (date->day < 1 || date->day > daysinMonth[date->month-1]) return 1;
  }

  return 0;
}
char* read_date_input(const char* prompt){
  char* date = malloc (11 * sizeof(char));
  Date curr_date = {0};
  int rc;
  while (1){
    printf("%s", prompt);
    if (scanf("%d-%d-%d", &curr_date.year, &curr_date.month, &curr_date.day ) == 3){
      getchar();
      printf("\n");
      rc = check_date_format(&curr_date);
      if (rc) {
        continue;
      } else {
        break;
      }
    }
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("Enter date in the currect format please\n");
  }
  sprintf(date, "%04d-%02d-%02d", curr_date.year, curr_date.month, curr_date.day);

  return date;
}
