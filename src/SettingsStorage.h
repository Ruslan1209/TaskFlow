#ifndef SETTINGSSTORAGE_H
#define SETTINGSSTORAGE_H
#include <string>
#include <fstream>
#include <sstream>
#include "TaskStorage.h"
using namespace std;

// абстрактный продукт второго семейства: файл настроек.
// Файл настроек всегда называется settings.txt и хранит две величины:
// путь к файлу задач и формат (txt/csv) — одну строкой: путь;формат
class ISettingsStorage {
public:
    virtual ~ISettingsStorage() {}
    virtual bool save(const string& taskPath, const string& format) const = 0;
    virtual bool load(string& taskPath, string& format) const = 0;
};

class TxtSettingsStorage : public ISettingsStorage {
public:
    bool save(const string& taskPath, const string& format) const {
        ofstream out("settings.txt");
        if (!out) return false;
        out << cleanText(taskPath) << ';' << cleanText(format) << "\n";
        return true;
    }
    bool load(string& taskPath, string& format) const {
        ifstream in("settings.txt");
        if (!in) return false;
        string line;
        if (!getline(in, line) || line.empty()) return false;
        istringstream iss(line);
        string p, f;
        if (!getline(iss, p, ';')) return false;
        getline(iss, f, ';');
        if (f != "csv") f = "txt";
        taskPath = p;
        format = f;
        return true;
    }
};

class CsvSettingsStorage : public ISettingsStorage {
public:
    // файл настроек маленький, формат строки тот же — разделитель ';'
    bool save(const string& taskPath, const string& format) const {
        ofstream out("settings.txt");
        if (!out) return false;
        out << cleanText(taskPath) << ';' << cleanText(format) << "\n";
        return true;
    }
    bool load(string& taskPath, string& format) const {
        ifstream in("settings.txt");
        if (!in) return false;
        string line;
        if (!getline(in, line) || line.empty()) return false;
        istringstream iss(line);
        string p, f;
        if (!getline(iss, p, ';')) return false;
        getline(iss, f, ';');
        if (f != "csv") f = "txt";
        taskPath = p;
        format = f;
        return true;
    }
};

#endif // SETTINGSSTORAGE_H
