#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "db.h"
#include "schema.h"
#include "sql_commands.h"
#include "mapper_func.h"

// ─── helpers ────────────────────────────────────────────────────────────────

static sqlite3 *setup() {
    sqlite3 *db;
    sqlite3_open(":memory:", &db);
    int rc = first_run(db);
    assert(rc == SQLITE_OK);
    return db;
}

static void teardown(sqlite3 *db) {
    sqlite3_close(db);
}

// Insert a task directly — bypasses sql_command_exec so we control column order
static void insert_test_task(sqlite3 *db, const char *name, int estimated_min) {
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "INSERT INTO tasks (name, estimated_min) VALUES (?, ?);", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 2, estimated_min);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

// ─── tests ──────────────────────────────────────────────────────────────────

void test_first_run_creates_tables() {
    sqlite3 *db = setup();

    // if tables exist, COUNT(*) won't error
    assert(count_rows(db, "SELECT COUNT(*) FROM subjects;")      >= 0);
    assert(count_rows(db, "SELECT COUNT(*) FROM daily_log;")     >= 0);
    assert(count_rows(db, "SELECT COUNT(*) FROM study_sessions;")>= 0);
    assert(count_rows(db, "SELECT COUNT(*) FROM tasks;")         >= 0);
    assert(count_rows(db, "SELECT COUNT(*) FROM bank_snapshots;")>= 0);
    assert(count_rows(db, "SELECT COUNT(*) FROM income_events;") >= 0);
    assert(count_rows(db, "SELECT COUNT(*) FROM expenses;")      >= 0);

    printf("PASS test_first_run_creates_tables\n");
    teardown(db);
}

void test_count_rows_empty() {
    sqlite3 *db = setup();

    assert(count_rows(db, count_subjects) == 0);
    assert(count_rows(db, count_tasks)    == 0);

    printf("PASS test_count_rows_empty\n");
    teardown(db);
}

void test_first_write_into_db() {
    sqlite3 *db = setup();

    int arr[7] = {
        1744200000,  // sleep_time
        1744228800,  // wake_time
        4,           // sleep_quality
        3,           // mood
        4,           // energy
        500000,      // bank balance in cents
        2            // number of subjects
    };
    char *sub_arr[] = { "Linear Algebra", "STM32" };

    int rc = first_write_into_db(db, arr, sub_arr);
    assert(rc == 0);

    assert(count_rows(db, count_subjects)                         == 2);
    assert(count_rows(db, "SELECT COUNT(*) FROM daily_log;")      == 1);
    assert(count_rows(db, "SELECT COUNT(*) FROM bank_snapshots;") == 1);

    printf("PASS test_first_write_into_db\n");
    teardown(db);
}

void test_get_rows_subjects() {
    sqlite3 *db = setup();

    int arr[7] = { 0, 0, 3, 3, 3, 100000, 2 };
    char *sub_arr[] = { "STM32", "RTOS" };
    first_write_into_db(db, arr, sub_arr);

    int num = count_rows(db, count_subjects);
    assert(num == 2);

    Subjects *subjects = malloc(num * sizeof(Subjects));
    int read = get_rows(db, get_subjects, (void *)subjects, map_subject);
    assert(read == 2);

    // check both names came back correctly
    int found_stm32 = 0, found_rtos = 0;
    for (int i = 0; i < read; i++) {
        if (strcmp(subjects[i].subject, "STM32") == 0) found_stm32 = 1;
        if (strcmp(subjects[i].subject, "RTOS")  == 0) found_rtos  = 1;
    }
    assert(found_stm32);
    assert(found_rtos);

    free(subjects);
    printf("PASS test_get_rows_subjects\n");
    teardown(db);
}

void test_insert_study_session() {
    sqlite3 *db = setup();

    // need a subject to satisfy the FK
    int arr[7] = { 0, 0, 3, 3, 3, 0, 1 };
    char *sub_arr[] = { "STM32" };
    first_write_into_db(db, arr, sub_arr);

    // start=100, end=3700, duration=60min, mood_before=3, mood_after=4,
    // energy_before=3, focus=4, subject=1, environment=0, distraction=2
    int session_arr[10] = { 100, 3700, 60, 3, 4, 3, 4, 1, 0, 2 };
    int rc = sql_command_exec(db, "study_sessions", insert_study_session, session_arr, 10, "good session");
    assert(rc == SQLITE_OK);

    assert(count_rows(db, "SELECT COUNT(*) FROM study_sessions;") == 1);

    // verify notes stored correctly
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT notes FROM study_sessions WHERE id = 1;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    const char *notes = (const char *)sqlite3_column_text(stmt, 0);
    assert(strcmp(notes, "good session") == 0);
    sqlite3_finalize(stmt);

    printf("PASS test_insert_study_session\n");
    teardown(db);
}

void test_task_insert_and_count() {
    sqlite3 *db = setup();

    insert_test_task(db, "Finish GPIO driver", 120);
    insert_test_task(db, "Read RTOS chapter",   60);

    assert(count_rows(db, count_tasks) == 2);

    printf("PASS test_task_insert_and_count\n");
    teardown(db);
}

void test_task_time_update() {
    sqlite3 *db = setup();
    insert_test_task(db, "Write SPI driver", 90);

    // update_task_time: SET observed_min = observed_min + ?1 WHERE id = ?2
    int arr[2] = { 45, 1 };  // add 45 mins to task id=1
    int rc = sql_command_exec(db, "tasks", update_task_time, arr, 2, NULL);
    assert(rc == SQLITE_OK);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT observed_min FROM tasks WHERE id = 1;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    assert(sqlite3_column_int(stmt, 0) == 45);
    sqlite3_finalize(stmt);

    // add another 30 mins
    arr[0] = 30;
    sql_command_exec(db, "tasks", update_task_time, arr, 2, NULL);
    sqlite3_prepare_v2(db, "SELECT observed_min FROM tasks WHERE id = 1;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    assert(sqlite3_column_int(stmt, 0) == 75);
    sqlite3_finalize(stmt);

    printf("PASS test_task_time_update\n");
    teardown(db);
}

void test_task_complete() {
    sqlite3 *db = setup();
    insert_test_task(db, "Finish DMA chapter", 60);

    assert(count_rows(db, count_tasks) == 1);  // incomplete count = 1

    int arr[1] = { 1 };  // task id=1
    int rc = sql_command_exec(db, "tasks", complete_task, arr, 1, NULL);
    assert(rc == SQLITE_OK);

    // should now be 0 incomplete tasks
    assert(count_rows(db, count_tasks) == 0);

    // verify completed flag
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "SELECT completed FROM tasks WHERE id = 1;", -1, &stmt, NULL);
    sqlite3_step(stmt);
    assert(sqlite3_column_int(stmt, 0) == 1);
    sqlite3_finalize(stmt);

    printf("PASS test_task_complete\n");
    teardown(db);
}

void test_get_rows_tasks() {
    sqlite3 *db = setup();
    insert_test_task(db, "GPIO driver", 120);
    insert_test_task(db, "RTOS lab",     60);

    int num = count_rows(db, count_tasks);
    assert(num == 2);

    Task *tasks = malloc(num * sizeof(Task));
    int read = get_rows(db, get_incomplete_tasks, (void *)tasks, map_tasks);
    assert(read == 2);

    int found_gpio = 0, found_rtos = 0;
    for (int i = 0; i < read; i++) {
        if (strcmp(tasks[i].task, "GPIO driver") == 0) { found_gpio = 1; assert(tasks[i].estimated_mins == 120); }
        if (strcmp(tasks[i].task, "RTOS lab")    == 0) { found_rtos = 1; assert(tasks[i].estimated_mins ==  60); }
    }
    assert(found_gpio);
    assert(found_rtos);

    free(tasks);
    printf("PASS test_get_rows_tasks\n");
    teardown(db);
}

// ─── main ───────────────────────────────────────────────────────────────────

int main() {
    test_first_run_creates_tables();
    test_count_rows_empty();
    test_first_write_into_db();
    test_get_rows_subjects();
    test_insert_study_session();
    test_task_insert_and_count();
    test_task_time_update();
    test_task_complete();
    test_get_rows_tasks();

    printf("\nAll tests passed.\n");
    return 0;
}
