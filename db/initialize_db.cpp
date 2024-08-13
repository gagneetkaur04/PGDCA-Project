#include <iostream>
#include <fstream>
#include <sqlite3.h>

void executeSQL(sqlite3* db, const std::string& sql) {
    char* errorMessage = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errorMessage);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    } else {
        std::cout << "SQL executed successfully." << std::endl;
    }
}

std::string readSQLFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open SQL file: " << filename << std::endl;
        return "";
    }
    std::string sql((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return sql;
}

int main() {
    sqlite3* db;
    int exit = sqlite3_open("db/todolist.db", &db);
    
    if (exit) {
        std::cerr << "Error in opening database: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    } else {
        std::cout << "Connected to the database successfully!" << std::endl;
    }

    std::string sql = readSQLFile("db/create_todolist_db.sql");
    if (!sql.empty()) {
        executeSQL(db, sql);
    }

    sqlite3_close(db);
    return 0;
}