const char* insert_daily_log = "INSERT INTO daily_log ("
        "    date, sleep_time, wake_time, sleep_dur_min, sleep_quality, "
        "    mood, energy, eat_out_meals, home_cooked, steps, "
        "    outside_min, screen_time_min, exercise, caffeine_drinks, "
        "    stress, productive_feel, notes"
        ") VALUES ("
        "    ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

const char* first_log = "INSERT INTO daily_log (date, sleep_time, wake_time, sleep_quality, mood, energy) "
        "VALUES (date('now', 'localtime'), ?, ?, ?, ?, ?);";

const char* first_bank_balance = "INSERT INTO bank_snapshots (date, balance_cents, computed_cents, discrepancy_cents) "
        "VALUES (date('now', 'localtime'), ?, ?, 0);";

const char* insert_subjects = "INSERT INTO subjects (name) VALUES (?);";

const char *insert_study_session =
    "INSERT INTO study_sessions (date, start_time, end_time, duration_min, "
    "mood_before, mood_after, energy_before, focus_rating, subject, "
    "environment, distraction_count, notes) "
    "VALUES (date('now', 'localtime'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

const char *insert_task =
    "INSERT INTO tasks (name, estimated_min) VALUES (?, ?);";

const char *update_task_time =
    "UPDATE tasks SET observed_min = observed_min + ? WHERE id = ?;";

const char *complete_task =
    "UPDATE tasks SET completed = 1 WHERE id = ?;";

const char *get_incomplete_tasks =
    "SELECT id, name, estimated_min, observed_min FROM tasks WHERE completed = 0;";

const char* count_subjects = "SELECT COUNT(*) FROM subjects;";

const char* count_tasks = "SELECT COUNT(*) FROM tasks WHERE completed = 0;";

const char* get_subjects = "SELECT id, name FROM subjects;";
