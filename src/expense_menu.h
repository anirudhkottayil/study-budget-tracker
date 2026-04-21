#ifndef EXPENSE_MENU_H
#define EXPENSE_MENU_H

#include <sqlite3.h>
#define wantScore 0.3
#define needScore 0.7

int expenses_menu(sqlite3* db, int* in_study);

#endif
