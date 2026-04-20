const char* insert_daily_log = "INSERT INTO daily_log ("
        "    date, sleep_time, wake_time, sleep_dur_min, sleep_quality, "
        "    mood, energy, eat_out_meals, home_cooked, steps, "
        "    outside_min, screen_time_min, exercise, caffeine_drinks, "
        "    stress, productive_feel, notes"
        ") VALUES ("
        "    ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

const char* first_log = 
    "INSERT INTO daily_log (date, sleep_time, wake_time, sleep_dur_min, sleep_quality, mood, energy) "
    "VALUES (date('now', 'localtime'), ?, ?, ?, ?, ?, ?);";

const char* first_bank_balance = "INSERT INTO bank_snapshots (date, balance_cents, computed_cents, discrepancy_cents) "
        "VALUES (date('now', 'localtime'), ?, ?, 0);";

const char* insert_subjects = "INSERT INTO subjects (name) VALUES (?);";

const char *insert_study_session =
    "INSERT INTO study_sessions (date, start_time, end_time, duration_min, "
    "mood_before, mood_after, energy_before, focus_rating, subject, "
    "environment, distraction_count, notes) "
    "VALUES (date('now', 'localtime'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

const char *insert_task =
    "INSERT INTO tasks (estimated_min, subject, name) VALUES (?, ?, ?);";
const char *update_task_time =
    "UPDATE tasks SET observed_min = observed_min + ? WHERE id = ?;";

const char *complete_task =
    "UPDATE tasks SET completed = 1 WHERE id = ?;";

const char *get_incomplete_tasks =
    "SELECT id, name, subject, estimated_min, observed_min FROM tasks WHERE completed = 0;";

const char* count_subjects = "SELECT COUNT(*) FROM subjects;";

const char* count_tasks = "SELECT COUNT(*) FROM tasks WHERE completed = 0;";

const char* get_subjects = "SELECT id, name FROM subjects;";

const char *count_daily_logs = "SELECT COUNT(*) FROM daily_log;";

const char* get_n_logs = "SELECT * FROM daily_log ORDER BY date DESC LIMIT ?;";

const char* get_log = "SELECT * FROM daily_log WHERE date = ?;";

const char *update_daily_log =
    "UPDATE daily_log SET "
    "sleep_time = ?, wake_time = ?, sleep_dur_min = ?, sleep_quality = ?, "
    "mood = ?, energy = ?, eat_out_meals = ?, home_cooked = ?, steps = ?, "
    "outside_min = ?, screen_time_min = ?, exercise = ?, caffeine_drinks = ?, "
    "stress = ?, productive_feel = ?, notes = ? "
    "WHERE date = ?;";

const char *count_expenses = "SELECT COUNT(*) FROM expenses;";

const char *get_expenses =
    "SELECT id, date, category, amount_cents, need_score, want_score, "
    "importance, recurrence, planned, payment_method, time_of_purchase, notes "
    "FROM expenses ORDER BY date DESC LIMIT ?;";

const char *update_expense =
    "UPDATE expenses SET "
    "category = ?, amount_cents = ?, need_score = ?, want_score = ?, "
    "importance = ?, recurrence = ?, planned = ?, payment_method = ?, "
    "time_of_purchase = ?, notes = ? "
    "WHERE date = ?;";

const char *delete_subject_sql = "DELETE FROM subjects WHERE name = ?;";

const char *count_income_events = "SELECT COUNT(*) FROM income_events;";

const char *get_income_events =
    "SELECT id, date, amount_cents, source, notes FROM income_events ORDER BY date DESC LIMIT ?;";

const char *insert_income =
    "INSERT INTO income_events (date, amount_cents, source, notes) "
    "VALUES (date('now', 'localtime'), ?, ?, ?);";

const char *update_income =
    "UPDATE income_events SET amount_cents = ?, source = ?, notes = ? "
    "WHERE date = ?;";

const char *get_last_log_date = "SELECT date FROM daily_log ORDER BY date DESC LIMIT 1;";

const char *update_eod_log =
    "UPDATE daily_log SET "
    "eat_out_meals = ?, home_cooked = ?, steps = ?, outside_min = ?, "
    "screen_time_min = ?, exercise = ?, caffeine_drinks = ?, "
    "stress = ?, productive_feel = ?, notes = ? "
    "WHERE date = ?;";

const char *insert_expense =
    "INSERT INTO expenses (category, amount_cents, need_score, want_score, "
    "importance, recurrence, planned, payment_method, time_of_purchase, notes, date) "
    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
