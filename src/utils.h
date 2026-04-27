#ifndef UTILS_H
#define UTILS_H

typedef struct {
  int day;
  int month;
  int year;
} Date;

int read_int_input(const char* prompt, int main, int max);
double read_double_input(const char* prompt, double main, double max);
char* read_date_input(const char* prompt);

#endif
