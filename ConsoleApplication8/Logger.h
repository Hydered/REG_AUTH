#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

class Logger {
public:
    static void log(const string& message) {
        ofstream file("server.log", ios::app);
        if (file.is_open()) {
            time_t now = time(0);
            char buf[26];
            ctime_s(buf, sizeof(buf), &now);
            file << buf << ": " << message << endl;
        }
    }
};
