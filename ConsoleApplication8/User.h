#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <openssl/sha.h>
#include <random>

using namespace std;

class User {
public:
    string login, password, salt, role;

    User(const string& lgn, const string& pswrd, const string& rl)
        : login(lgn), salt(generateSalt()), role(rl) {
        password = sha256(pswrd, salt);
    }

    static string generateSalt() {
        string slt;
        static const char alphanum[] =
            "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        srand(static_cast<unsigned>(time(nullptr)));
        for (int i = 0; i < 16; ++i)
            slt += alphanum[rand() % (sizeof(alphanum) - 1)];
        return slt;
    }

    static string sha256(const string& input, const string& salt) {
        string salted_input = input + salt;
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, salted_input.c_str(), salted_input.size());
        SHA256_Final(hash, &sha256);

        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << hex << setw(2) << setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    bool auth(const string& input_pass) const {
        return sha256(input_pass, salt) == password;
    }

    virtual bool hasAccess(const string& res) const { return false; }
    virtual ~User() = default;
};

class Admin : public User {
public:
    Admin(const string& lgn, const string& pswrd) : User(lgn, pswrd, "Admin") {}
    bool hasAccess(const string&) const override { return true; }
};

class Regular_user : public User {
public:
    Regular_user(const string& lgn, const string& pswrd) : User(lgn, pswrd, "User") {}
    bool hasAccess(const string& res) const override {
        return res == "Profile" || res == "Public";
    }
};

class Guest : public User {
public:
    Guest() : User("Guest", "", "Guest") {
        salt = "";
        password = "";
    }
    bool hasAccess(const string& res) const override {
        return res == "Public";
    }
};