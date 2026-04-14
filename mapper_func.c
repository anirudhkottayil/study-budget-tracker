#include "db.h"
#include <string.h>

void map_subject(sqlite3_stmt* ppStmt, void* arr, int index){
  Subjects* subjects = (Subjects*) arr;
      subjects[index].id = sqlite3_column_int (ppStmt, 0);
      strncpy(subjects[index].subject,(const char*) sqlite3_column_text(ppStmt, 1), 49);
      subjects[index].subject[49] = '\0'; 
}
