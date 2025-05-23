#pragma once
#include <iostream>
#include "sqlite3.h"
#include "User.h"
#include <vector>
#include <memory>

using namespace std;

class DatabaseManager {
private:
    sqlite3* db;
public:
    DatabaseManager(const string& path) {
        if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
            cerr << "Can't open database" << endl;
        }
        else {
            createTable();
        }
    }
    ~DatabaseManager() { sqlite3_close(db); }

    void createTable() {
        const char* sql = "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "salt TEXT NOT NULL,"
            "role TEXT NOT NULL);";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "Table creation failed: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
    }

    bool saveUser(const string& login, const string& password, const string& salt, const string& role) {
        const char* sql = "INSERT INTO users(username, password_hash, salt, role) VALUES (?,?,?,?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
            return false;

        sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, salt.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, role.c_str(), -1, SQLITE_TRANSIENT);

        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return success;
    }

    vector<unique_ptr<User>> loadUsers() {
        vector<unique_ptr<User>> users;
        const char* sql = "SELECT username, password_hash, salt, role FROM users;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                string login = (const char*)sqlite3_column_text(stmt, 0);
                string pass = (const char*)sqlite3_column_text(stmt, 1);
                string salt = (const char*)sqlite3_column_text(stmt, 2);
                string role = (const char*)sqlite3_column_text(stmt, 3);

                unique_ptr<User> u;
                if (role == "Admin")
                    u = make_unique<Admin>(login, pass);
                else if (role == "User")
                    u = make_unique<Regular_user>(login, pass);
                else
                    u = make_unique<Guest>();

                u->salt = salt;
                u->password = pass;
                users.push_back(move(u));
            }
        }
        sqlite3_finalize(stmt);
        return users;
    }
};
