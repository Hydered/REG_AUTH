#pragma once
#include <vector>
#include <memory>
#include <string>
#include "User.h"
#include "DatabaseManager.h"

using namespace std;

class UserManager {
private:
    vector<unique_ptr<User>> users;
    DatabaseManager db;

public:
    UserManager() : db("auth.db") {
        users = db.loadUsers();
    }

    void addUser(unique_ptr<User> user) {
        db.saveUser(user->login, user->password, user->salt, user->role);
        users.push_back(move(user));
    }

    bool auth(const string& uname, const string& pass) {
        for (auto& user : users)
            if (user->login == uname && user->auth(pass))
                return true;
        return false;
    }

    bool checkAccess(const string& uname, const string& resource) {
        for (auto& user : users)
            if (user->login == uname)
                return user->hasAccess(resource);
        return false;
    }

    bool exists(const string& uname) {
        for (auto& user : users)
            if (user->login == uname)
                return true;
        return false;
    }
};
