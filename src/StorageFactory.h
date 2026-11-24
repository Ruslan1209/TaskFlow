#ifndef STORAGEFACTORY_H
#define STORAGEFACTORY_H
#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include "TaskStorage.h"
using namespace std;

// --- второй продукт семейства: хранилище настроек ---
// оба семейства пишут в один файл settings.txt, но разметка своя:
// txt — строки «ключ значение», csv — заголовок и разделитель ';'

class ISettingsStorage {
public:
    virtual ~ISettingsStorage() {}
    virtual bool save(const string& path, const string& format) const = 0;
    virtual bool load(string& path, string& format) const = 0;
};

class TxtSettingsStorage : public ISettingsStorage {
public:
    bool save(const string& path, const string& format) const {
        ofstream out("settings.txt");
        if (!out) return false;
        out << "path " << path << "\nformat " << format << "\n";
        return true;
    }
    // возвращает true, только если нашёл знакомые ключи —
    // тогда csv-разметку читать не нужно
    bool load(string& path, string& format) const {
        ifstream in("settings.txt");
        if (!in) return false;
        bool found = false;
        string line;
        while (getline(in, line)) {
            size_t sp = line.find(' ');
            if (sp == string::npos) continue;
            string key = line.substr(0, sp), value = line.substr(sp + 1);
            if (key == "path") { path = value; found = true; }
            else if (key == "format") { format = value; found = true; }
        }
        return found;
    }
};

class CsvSettingsStorage : public ISettingsStorage {
public:
    bool save(const string& path, const string& format) const {
        ofstream out("settings.txt");
        if (!out) return false;
        out << "ключ;значение\n";
        out << "path;" << path << "\n";
        out << "format;" << format << "\n";
        return true;
    }
    bool load(string& path, string& format) const {
        ifstream in("settings.txt");
        if (!in) return false;
        bool found = false;
        string line;
        while (getline(in, line)) {
            istringstream iss(line);
            string key, value;
            if (!getline(iss, key, ';')) continue;
            if (!getline(iss, value)) continue;
            if (key == "path") { path = value; found = true; }
            else if (key == "format") { format = value; found = true; }
        }
        return found;
    }
};

// --- Abstract Factory: семейство «формат хранения» ---
// txt-семейство: TxtTaskStorage + TxtSettingsStorage
// csv-семейство: CsvTaskStorage + CsvSettingsStorage
class IStorageFactory {
public:
    virtual ~IStorageFactory() {}
    virtual string formatName() const = 0;
    virtual unique_ptr<ITaskStorage> createTaskStorage() const = 0;
    virtual unique_ptr<ISettingsStorage> createSettingsStorage() const = 0;
};

class TxtStorageFactory : public IStorageFactory {
public:
    string formatName() const { return "txt"; }
    unique_ptr<ITaskStorage> createTaskStorage() const {
        return unique_ptr<ITaskStorage>(new TxtTaskStorage());
    }
    unique_ptr<ISettingsStorage> createSettingsStorage() const {
        return unique_ptr<ISettingsStorage>(new TxtSettingsStorage());
    }
};

class CsvStorageFactory : public IStorageFactory {
public:
    string formatName() const { return "csv"; }
    unique_ptr<ITaskStorage> createTaskStorage() const {
        return unique_ptr<ITaskStorage>(new CsvTaskStorage());
    }
    unique_ptr<ISettingsStorage> createSettingsStorage() const {
        return unique_ptr<ISettingsStorage>(new CsvSettingsStorage());
    }
};

// выбор семейства по формату
inline unique_ptr<IStorageFactory> makeStorageFactory(const string& format) {
    if (format == "csv")
        return unique_ptr<IStorageFactory>(new CsvStorageFactory());
    return unique_ptr<IStorageFactory>(new TxtStorageFactory());
}

#endif // STORAGEFACTORY_H
