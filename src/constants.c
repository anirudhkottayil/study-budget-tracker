#include<stdio.h>
const char *categories[] = {
      "Food", "Transport", "Groceries", "Utilities", "Rent",
      "Health", "Education", "Entertainment", "Clothing", "Other"
  };

  const char *recurrence_str[] = { "One-off", "Weekly","Fortnighly" ,"Monthly" };

  const char *payment_str[]    = { "Cash", "Card", "Tap" };

  const char *sources[] = { "Salary", "Freelance", "Transfer", "Other" };

int print_category(){
  int i = 0;
  printf("| %-3s | %-15s |\n", "ID", "CATEGORY");
  printf("|-----|------------------|\n");
  for (; i < 10; i++)
      printf("| %-3d | %-15s |\n", i, categories[i]);
  return i;
}

int print_recurrence(){
  int i = 0;
  printf("| %-3s | %-10s |\n", "ID", "RECURRENCE");
  printf("|-----|------------|\n");
  for (; i < 4; i++)
      printf("| %-3d | %-10s |\n", i, recurrence_str[i]);
  return i;
}

int print_payment(){
  int i = 0;
  printf("| %-3s | %-10s |\n", "ID", "PAYMENT");
  printf("|-----|------------|\n");
  for (; i < 3; i++)
      printf("| %-3d | %-10s |\n", i, payment_str[i]);
  return i;
}
