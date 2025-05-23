#include <iostream>
#include "UserManager.h"
#include "Logger.h"

using namespace std;

int main() {

    setlocale(LC_ALL, "Russian");

    UserManager manager;
    string command;

    cout << "Доступные команды:\n";
    cout << "  REGISTER:<логин>:<пароль>:<роль>     - регистрация нового пользователя\n";
    cout << "  LOGIN:<логин>:<пароль>               - вход в систему\n";
    cout << "  CHECK_ACCESS:<логин>:<ресурс>        - проверка доступа к ресурсу\n";
    cout << "\nПримеры:\n";
    cout << "  REGISTER:admin:1234:Admin\n";
    cout << "  LOGIN:admin:1234\n";
    cout << "  CHECK_ACCESS:admin:Profile\n\n";

    while (true) {
        cout << "\nВведите команду: ";
        if (!getline(cin, command) or command.empty()) continue;

        if (command.find("LOGIN:") == 0) {
            auto p1 = command.find(":", 6);
            string user = command.substr(6, p1 - 6);
            string pass = command.substr(p1 + 1);
            if (manager.auth(user, pass)) {
                cout << "ВХОД ВЫПОЛНЕН УСПЕШНО\n";
                Logger::log("Login success: " + user);
            }
            else {
                cout << "ОШИБКА ВХОДА: Неверный логин или пароль\n";
                Logger::log("Login failed: " + user);
            }

        }
        else if (command.find("CHECK_ACCESS:") == 0) {
            auto p1 = command.find(":", 13);
            string user = command.substr(13, p1 - 13);
            string resource = command.substr(p1 + 1);
            if (manager.checkAccess(user, resource)) {
                cout << "ДОСТУП РАЗРЕШЕН\n";
            }
            else {
                cout << "ДОСТУП ЗАПРЕЩЕН\n";
            }

        }
        else if (command.find("REGISTER:") == 0) {
            auto p1 = command.find(":", 9);
            auto p2 = command.find(":", p1 + 1);
            string user = command.substr(9, p1 - 9);
            string pass = command.substr(p1 + 1, p2 - p1 - 1);
            string role = command.substr(p2 + 1);

            if (!manager.exists(user)) {
                if (role == "Admin")
                    manager.addUser(make_unique<Admin>(user, pass));
                else if (role == "User")
                    manager.addUser(make_unique<Regular_user>(user, pass));
                else
                    manager.addUser(make_unique<Guest>());

                cout << "РЕГИСТРАЦИЯ УСПЕШНА\n";
            }
            else {
                cout << "ПОЛЬЗОВАТЕЛЬ УЖЕ СУЩЕСТВУЕТ\n";
            }

        }
        else {
            cout << "Неизвестная команда. Повторите попытку.\n";
        }
    }
}
