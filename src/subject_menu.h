#ifndef SUBJECT_MENU_H
#define SUBJECT_MENU_H

#include <sqlite3.h>
#include "db.h"

int subjects_menu(sqlite3*db, int* in_study, Subjects** subjects, int* num_subjects);
int view_subjects(Subjects** subjects, int* num_subjects, int* in_study);


#endif
