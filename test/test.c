#include <assert.h>
#include <stdio.h>
#include "db.h"
#include "sql_commands.h"
#include "schema.h"

sqlite3 *setup() {
    sqlite3 *db;
    sqlite3_open(":memory:", &db);
    first_run(db);  // creates all tables
    return db;
}

void teardown(sqlite3 *db) {
    sqlite3_close(db);
}

void test_count_rows_empty() {
    sqlite3 *db = setup();
    int count = count_rows(db, "SELECT COUNT(*) FROM subjects;");
    assert(count == 0);
    printf("PASS test_count_rows_empty\n");
    teardown(db);
}

// void test_insert_subject() {
//     sqlite3 *db = setup();
//
//     int arr[1] = {0};
//     char *subjects[] = {"Linear Algebra"};
//     first_write_into_db(db, subjects, 1);  // or however you insert
//
//     int count = count_rows(db, "SELECT COUNT(*) FROM subjects;");
//     assert(count == 1);
//     printf("PASS test_insert_subject\n");
//     teardown(db);
// }

void test_study_session_insert() {
    sqlite3 *db = setup();

    int arr[10] = {1744000000, 1744003600, 60, 3, 4, 3, 4, 1, 0, 2};
    int rc = sql_command_exec(db, "study_sessions", insert_study_session, arr, 10, "good session");
    assert(rc == SQLITE_OK);

    int count = count_rows(db, "SELECT COUNT(*) FROM study_sessions;");
    assert(count == 1);
    printf("PASS test_study_session_insert\n");
    teardown(db);
}

void test_task_time_update() {
    sqlite3 *db = setup();

    // insert a task first
    int insert_arr[2] = {0};
    sql_command_exec(db, "tasks", insert_task, insert_arr, 0, "Finish STM32 GPIO");

    // update observed time
    int arr[2] = {60};
    sql_command_exec(db, "tasks", update_task_time, arr, 2, NULL);

    // verify
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT observed_min FROM tasks WHERE id = 1;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    int observed = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    assert(observed == 60);
    printf("PASS test_task_time_update\n");
    teardown(db);
}

int main() {
    test_count_rows_empty();
    test_insert_subject();
    test_study_session_insert();
    test_task_time_update();
    printf("\nAll tests passed.\n");
    return 0;
}
