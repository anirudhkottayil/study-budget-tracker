#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "expense_menu.h"
#include "mapper_func.h"
#include "db.h"
#include "sql_commands.h"
#include "constants.h"
#include "utils.h"

int update_account(sqlite3* db, char* date, int* in_study ) {
  if (*in_study){
    printf("In study session\n");
  }
  int arr[9];
  char notes[500];

  int cat_index = print_category();
  arr[0] = read_int_input("Enter category: ", 0, cat_index);
  double amount = read_double_input("Enter amount (e.g. 15.00): ", 1.0, 10000000.0);
  arr[1] = (int)round(amount * 100);
  arr[2] = read_int_input("Enter need score (1-5): ", 1, 5);
  arr[3] = read_int_input("Enter want score (1-5): ", 1, 5);
  // importance computed from need and want, scaled to int (*100)
  // store as int here, cast to double on insert
  // printf("Enter importance (0.00 - 1.00, e.g. 75 = 0.75): ");
  double importance = ((arr[2] * needScore) + (arr[3] * wantScore)) / 5;
  arr[4] = (int)round(importance * 100);
  int rec_index = print_recurrence();
  arr[5] = read_int_input("Enter recurrence: ", 0, rec_index);
  arr[6] = read_int_input("Planned? (1 = yes, 0 = no): ", 0, 1);
  int pay_index = print_payment();
  arr[7] = read_int_input("Enter payment method: ", 0, pay_index);
  arr[8] = (int)time(NULL);  // time_of_purchase

  printf("Enter notes: ");
  fgets(notes, 500, stdin);
  notes[strcspn(notes, "\n")] = '\0';
  printf("\n");

  int rc = sql_command_exec(db, update_expense, arr, 9, notes, date);
  if (rc){
    fprintf(stderr, "Error updating the account\n");
    return 1;
  }

  return 0;
}

void print_expenses(Expense *expenses, int count, int* in_study) {
  if (*in_study){
    printf("In Study Session\n");
  }
    for (int i = 0; i < count; i++) {
        char time_buf[32];
        time_t t = expenses[i].time_of_purchase;
        strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M", localtime(&t));

        int cat = expenses[i].category;
        int rec = expenses[i].recurrence;
        int pay = expenses[i].payment_method;

        printf("┌─────────────────────────────────────┐\n");
        printf("│  ID:             %d\n",    expenses[i].id);
        printf("│  Date:           %s\n",    expenses[i].date);
        printf("│  Category:       %s\n",    (cat >= 0 && cat <= 9) ? categories[cat] : "Unknown");
        printf("│  Amount:         $%.2f\n", expenses[i].amount_cents / 100.0);
        printf("│  Need Score:     %d/5\n",  expenses[i].need_score);
        printf("│  Want Score:     %d/5\n",  expenses[i].want_score);
        printf("│  Importance:     %.2f\n",  (float) expenses[i].importance / 100);
        printf("│  Recurrence:     %s\n",    (rec >= 0 && rec <= 3) ? recurrence_str[rec] : "Unknown");
        printf("│  Planned:        %s\n",    expenses[i].planned ? "Yes" : "No");
        printf("│  Payment Method: %s\n",    (pay >= 0 && pay <= 2) ? payment_str[pay]    : "Unknown");
        printf("│  Time:           %s\n",    time_buf);
        printf("│  Notes:          %s\n",    expenses[i].notes[0] ? expenses[i].notes : "—");
        printf("└─────────────────────────────────────┘\n");
    }
}
int count_accounts(sqlite3* db){
  int db_rows = count_rows(db, count_expenses);
  if ( db_rows == -1){
    fprintf(stderr, "Error getting row count for expenses\n");
    return -1;
  }
  return db_rows;
}

int view_accounts(sqlite3* db, int input, int* in_study){
  int arr[1]= {input};

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
    user_input = read_int_input("Enter your choice: ", 1, 3);

    if (user_input == 1){
      int account_count = count_accounts(db);
      if (account_count == -1) return 1;
      if (account_count == 0) {
        printf("No records to show\n");
      } else {
        if_input = read_int_input("How many previous days accounts do you want to see : ", 1, account_count);
        rc = view_accounts(db, if_input, in_study);
        if (rc) {
          return 1;
        }

      }
    } else if (user_input == 2){
      printf("Coming soon !\n");
      // char date[11];
      // printf("Enter date of the account you want to update (YYYY-MM-DD): ");
      // scanf("%10s", date);
      // getchar();
      // date[10] = '\0';
      // printf("\n");
      // rc = update_account(db, date, in_study);
      // if (rc) {
      //   return 1;
      // }
    } else if (user_input == 3){
      break;

    } else {
      printf("Invalid input!! Try again\n");
    }
  }
  return 0;

}


