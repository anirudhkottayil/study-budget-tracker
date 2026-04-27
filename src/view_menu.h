#ifndef VIEW_MENU_H
#define VIEW_MENU_H

#include "db.h"
void view_subjects(Subjects** subjects, int* num_subjects, int* in_study);

void view_tasks(Task** tasks, int* num_tasks, int* in_study, Subjects** subjects, int* num_subjects);

#endif
