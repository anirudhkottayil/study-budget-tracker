#ifndef TASKS_MENU_H
#define TASKS_MENU_H

#include <sqlite3.h>
#include "db.h" 
int tasks_menu(sqlite3* db, int* in_study, Task** tasks, int* num_tasks, Subjects** subjects, int* num_subjects);

#endif
