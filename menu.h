#ifndef MENU_H
#define MENU_H
#include "db.h"
int main_menu(int* in_study);
int study_menu(sqlite3* db, int* in_study, int* study_start, int* study_stop, int* distraction_count, Subjects* subjects, int num);

#endif
