CREATE TABLE tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_name TEXT NOT NULL,
    due_date DATE,
    is_completed BOOLEAN DEFAULT 0
);