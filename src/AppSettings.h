#ifndef APPSETTINGS_H
#define APPSETTINGS_H
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

// Singleton: единственные настройки программы (путь к файлу задач и формат).
// Конструктор закрыт, копирование запрещено — второй экземпляр создать нельзя.
class AppSettings {
private:
    string pathToFile;   // куда сохранять задачи
    string format;       // "txt" или "csv"

    // закрытый конструктор: объект создаёт только сам класс
    AppSettings() : pathToFile("tasks.txt"), format("txt") {
        load();
    }

    // копирование запрещено — иначе Singleton можно обойти через копию
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;

    // настройки переживают перезапуск: читаем settings.txt при первом обращении
    void load() {
        ifstream in("settings.txt");
        if (!in) return;
        string line;
        while (getline(in, line)) {
            size_t sp = line.find(' ');
            if (sp == string::npos) continue;
            string key = line.substr(0, sp);
            string value = line.substr(sp + 1);
            if (key == "path") pathToFile = value;
            else if (key == "format") format = value;
        }
        if (format != "txt" && format != "csv") format = "txt"; // защита от порчи файла
    }

    void save() {
        ofstream out("settings.txt");
        out << "path " << pathToFile << "\n";
        out << "format " << format << "\n";
    }

public:
    // Meyers' Singleton: первый вызов создаёт объект, дальше — тот же экземпляр
    static AppSettings& instance() {
        static AppSettings inst;
        return inst;
    }

    const string& getPathToFile() const { return pathToFile; }
    const string& getFormat() const     { return format; }

    void setPathToFile(const string& p) {
        if (!p.empty()) { pathToFile = p; save(); }
    }

    void setFormat(const string& f) {
        if (f == "txt" || f == "csv") { format = f; save(); }
        else cout << "Неверный формат, допустимы только txt и csv.\n";
    }
};

#endif // APPSETTINGS_H
