#ifndef STUDY_MENU_H
#define STUDY_MENU_H
#include "db.h"
int main_menu(int* in_study);
int study_menu(sqlite3* db, int* in_study, int* study_start, int* study_stop, int* distraction_count, Subjects** subjects, int* num_subject, Task** task, int* num_task);

#endif
