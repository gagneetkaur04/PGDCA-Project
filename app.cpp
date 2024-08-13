#include <iostream>
#include <iomanip>
#include <sqlite3.h>
#include <string>
#include <ctime>

using namespace std;

class TaskManager {
private:
    sqlite3* db;

public:
    TaskManager(const string& dbPath) {
        int exit = sqlite3_open(dbPath.c_str(), &db);
        if (exit) {
            cerr << "Error in opening database: " << sqlite3_errmsg(db) << endl;
            db = nullptr;
        } 
    }

    ~TaskManager() {
        if (db) {
            sqlite3_close(db);
        }
    }

    void addTask(const string& taskName, const string& dueDate) {
        if (!db) return;

        string sql = "INSERT INTO tasks (task_name, due_date) VALUES ('" + taskName + "', '" + dueDate + "');";
        char* errorMessage = nullptr;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errorMessage << endl;
            sqlite3_free(errorMessage);
        } else {
            cout << "Task added successfully!" << endl;
        }
    }

    void viewTasks(bool onlyPending = false) {
        if (!db) return;

        string sql = "SELECT * FROM tasks";
        if (onlyPending) {
            sql += " WHERE is_completed = 0";
        }
        sql += " ORDER BY strftime('%Y-%m-%d', due_date) ASC;";

        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << endl;
            return;
        }

        bool hasTasks = false;
        cout << left << setw(5) << "ID" 
                  << setw(30) << "Task Name" 
                  << setw(15) << "Due Date" 
                  << setw(15) << "Completed" << endl;
        cout << "-------------------------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasTasks = true;
            int id = sqlite3_column_int(stmt, 0);
            const char* taskName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* dueDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            bool isCompleted = sqlite3_column_int(stmt, 3);

            cout << left << setw(5) << id
                      << setw(30) << taskName 
                      << setw(15) << dueDate 
                      << setw(15) << (isCompleted ? "Yes" : "No") << endl;
        }

        if (!hasTasks) {
            cout << "No Tasks Due" << endl;
        }

        sqlite3_finalize(stmt);
    }

    void completeTask(int taskId) {
        if (!db) return;

        string sql = "UPDATE tasks SET is_completed = 1 WHERE id = " + to_string(taskId) + ";";
        char* errorMessage = nullptr;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errorMessage << endl;
            sqlite3_free(errorMessage);
        } else {
            cout << "Task marked as completed!" << endl;
        }
    }

    void deleteTask(int taskId) {
        if (!db) return;

        string sql = "DELETE FROM tasks WHERE id = " + to_string(taskId) + ";";
        char* errorMessage = nullptr;

        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errorMessage << endl;
            sqlite3_free(errorMessage);
        } else {
            cout << "Task deleted!" << endl;
        }
    }

    bool checkForOverdueTasks() {
        if (!db) return false;

        time_t now = time(0);
        tm* ltm = localtime(&now);
        char currentDate[11];
        strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", ltm);

        string sql = "SELECT COUNT(*) FROM tasks WHERE due_date < '" + string(currentDate) + "' AND is_completed = 0;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        int count = 0;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }

        sqlite3_finalize(stmt);
        return count > 0;
    }

    void showOverdueTasks() {
        if (!db) return;

        time_t now = time(0);
        tm* ltm = localtime(&now);
        char currentDate[11];
        strftime(currentDate, sizeof(currentDate), "%Y-%m-%d", ltm);

        string sql = "SELECT * FROM tasks WHERE due_date < '" + string(currentDate) + "' AND is_completed = 0 ORDER BY due_date ASC;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << endl;
            return;
        }

        bool hasOverdueTasks = false;
        cout << endl;
        cout << ">>>>>>>>>>>>>>>>>>>>>>>> OVERDUE TASKS <<<<<<<<<<<<<<<<<<<<<<" << endl << endl;
        cout << left << setw(5) << "ID" 
                  << setw(30) << "Task Name" 
                  << setw(15) << "Due Date" 
                  << setw(15) << "Completed" << endl;
        cout << "-------------------------------------------------------------" << endl;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasOverdueTasks = true;
            int id = sqlite3_column_int(stmt, 0);
            const char* taskName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* dueDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            bool isCompleted = sqlite3_column_int(stmt, 3);

            cout << left << setw(5) << id
                      << setw(30) << taskName 
                      << setw(15) << dueDate 
                      << setw(15) << (isCompleted ? "Yes" : "No") << endl;
        }

        cout << "-------------------------------------------------------------" << endl << endl;

        if (!hasOverdueTasks) {
            cout << "No Overdue Tasks" << endl;
        }

        sqlite3_finalize(stmt);
    }
};

void showMenu() {
    cout << endl;
    cout << "===========================================" << endl;
    cout << "           TODO LIST APPLICATION" << endl;
    cout << "===========================================" << endl << endl;
    cout << "CHOOSE ONE OPTION: " << endl << endl;
    cout << "1. Add Task" << endl;
    cout << "2. View All Tasks" << endl;
    cout << "3. View Pending Tasks" << endl;
    cout << "4. Complete Task" << endl;
    cout << "5. Delete Task" << endl;
    cout << "6. Exit" << endl << endl;
    cout << "===========================================" << endl << endl;
}

bool promptShowMenu() {
    char choice;
    cout << endl;
    cout << "Go back to Menu (M)" << endl;
    cout << "Exit App (E)" << endl;
    cin >> choice;
    system("clear");
    return (choice == 'm' || choice == 'M');
}

int main() {
    TaskManager taskManager("db/todolist.db");

    if (taskManager.checkForOverdueTasks()) {
        taskManager.showOverdueTasks();
    }

    bool showMenuFlag = true;
    int choice;

    while (true) {
        if (showMenuFlag) {
            showMenu();
        }

        cin >> choice;
        system("clear");

        switch (choice) {
            case 1: {
                string taskName, dueDate;
                cout << endl;
                cout << "Enter task name: ";
                cin.ignore();
                getline(cin, taskName);
                cout << "Enter due date (YYYY-MM-DD): ";
                cin >> dueDate;
                taskManager.addTask(taskName, dueDate);
                break;
            }
            case 2:
                taskManager.viewTasks();
                break;
            case 3:
                taskManager.viewTasks(true);
                break;
            case 4: {
                cout << "Here are the current tasks:" << endl;
                taskManager.viewTasks();

                int taskId;
                cout << endl;
                cout << "Enter task ID to complete: ";
                cin >> taskId;
                taskManager.completeTask(taskId);
                break;
            }
            case 5: {
                cout << "Here are the current tasks:" << endl;
                taskManager.viewTasks();

                int taskId;
                cout << endl;
                cout << "Enter task ID to delete: ";
                cin >> taskId;
                taskManager.deleteTask(taskId);
                break;
            }
            case 6:
                cout << "Application Closed." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }

        if (!promptShowMenu()) {
            cout << "Application Closed." << endl;
            break;
        }
    }

    return 0;
}