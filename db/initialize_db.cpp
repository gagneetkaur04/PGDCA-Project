#include <iostream>
#include <fstream>
#include <sqlite3.h>

using namespace std;

void executeSQL(sqlite3* db, const string& sql) {
    char* errorMessage = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errorMessage << endl;
        sqlite3_free(errorMessage);
    } else {
        cout << "SQL executed successfully." << endl;
    }
}

string readSQLFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open SQL file: " << filename << endl;
        return "";
    }
    string sql((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    return sql;
}

int main() {
    sqlite3* db;
    int exit = sqlite3_open("db/todolist.db", &db);
    
    if (exit) {
        cerr << "Error in opening database: " << sqlite3_errmsg(db) << endl;
        return -1;
    } else {
        cout << "Connected to the database successfully!" << endl;
    }

    string sql = readSQLFile("db/create_todolist_db.sql");
    if (!sql.empty()) {
        executeSQL(db, sql);
    }

    sqlite3_close(db);
    return 0;
}