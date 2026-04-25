#include <stdio.h>
#include "utils.h"

int read_int_input(const char* prompt, int min, int max){
  int val = 0;
  while (1) {
    printf("%s (%d - %d): ", prompt, min, max);
    if ( scanf("%d", &val) == 1 && val >= min && val <= max){
      printf("\n");
      break;
    }
    printf("\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    printf("Enter values in the range please\n");
  }
  getchar();

  return val;
}
