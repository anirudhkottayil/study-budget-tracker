#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "expense_menu.h"
#include "mapper_func.h"
#include "db.h"
#include "sql_commands.h"

void print_expenses(Expense *expenses, int count, int* in_study) {
  if (*in_study){
    printf("In Study Session\n");
  }
    const char *categories[] = {
        "Food", "Transport", "Groceries", "Utilities",
        "Health", "Education", "Entertainment", "Clothing", "Other"
    };
    const char *recurrence_str[] = { "One-off", "Weekly", "Monthly" };
    const char *payment_str[]    = { "Cash", "Card", "Tap" };

    for (int i = 0; i < count; i++) {
        char time_buf[32];
        time_t t = expenses[i].time_of_purchase;
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", localtime(&t));

        printf("┌─────────────────────────────────────┐\n");
        printf("│  ID:             %d\n",    expenses[i].id);
        printf("│  Date:           %s\n",    expenses[i].date);
        printf("│  Category:       %s\n",    categories[(expenses[i].category >= 0 && expenses[i].category <= 8) ? expenses[i].category : 8]);
        printf("│  Amount:         $%.2f\n", expenses[i].amount_cents / 100.0);
        printf("│  Need Score:     %d/5\n",  expenses[i].need_score);
        printf("│  Want Score:     %d/5\n",  expenses[i].want_score);
        printf("│  Importance:     %.2f\n",  expenses[i].importance);
        printf("│  Recurrence:     %s\n",    (i >= 0 && i <= 2) ? recurrence_str[expenses[i].recurrence] : "Unkown");
        printf("│  Planned:        %s\n",    expenses[i].planned ? "Yes" : "No");
        printf("│  Payment Method: %s\n",    (i >= 0 && i <= 2) ? payment_str[expenses[i].payment_method] : "Unkown");
        printf("│  Time:           %s\n",    time_buf);
        printf("│  Notes:          %s\n",    expenses[i].notes[0] ? expenses[i].notes : "—");
        printf("└─────────────────────────────────────┘\n");
    }
}

int view_accounts(sqlite3* db, int input, int* in_study){
  int db_rows = count_rows(db, count_expenses);
  if ( db_rows == -1){
    fprintf(stderr, "Error getting row count for expenses\n");
    return 1;
  }
  int arr[1]= {(input > db_rows) ? db_rows : input};

  Expense* accounts = malloc(arr[0] * sizeof(Expense));
  int rc = get_rows(db, get_expenses,accounts, map_expense , arr, 1) ;
  if (rc == -1){
    fprintf(stderr, "Error getting rows for expenses\n");
    return 1;
  }

  print_expenses(accounts, arr[0], in_study);
  free(accounts);
  return 0;
}

int expenses_menu(sqlite3* db, int* in_study){
  int user_input ; int rc; int if_input;
  while (1){
    if (*in_study){
      printf("__EXPENSE_MENU___ (IN STUDY SESH)\n");
    } else {
      printf("___EXPENSE_MENU___\n");
    }
    printf("Enter 1 to view accounts\n");
    printf("Enter 2 to update accounts\n");
    printf("Enter 3 to go back to main menu\n");
    scanf("%d", &user_input);
    getchar();

    if (user_input == 1){
      printf("How many previous days accounts do you want to see : ");
      getchar();
      printf("\n");
      rc = view_accounts(db, if_input, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 2){
      char date[11];
      printf("Enter date of the account you want to update (YYYY-MM-DD): ");
      scanf("%10s", date);
      getchar();
      date[10] = '\0';
      printf("\n");
      rc = update_account(db, date, in_study);
      if (rc) {
        return 1;
      }
    } else if (user_input == 3){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }
  return 0;

}


