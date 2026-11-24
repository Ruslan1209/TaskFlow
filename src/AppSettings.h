#ifndef APPSETTINGS_H
#define APPSETTINGS_H
#include <string>
#include <iostream>
#include <memory>
#include "StorageFactory.h"
using namespace std;

// Singleton настроек (шаг 2). В шаге 5 стал клиентом Abstract Factory:
// файл настроек пишет и читает хранилище выбранного семейства.
class AppSettings {
private:
    string pathToFile;
    string format;
    unique_ptr<IStorageFactory> factory_;

    AppSettings() : pathToFile("tasks.txt"), format("txt") {
        load();
        factory_ = makeStorageFactory(format);
    }
    AppSettings(const AppSettings&) = delete;
    AppSettings& operator=(const AppSettings&) = delete;

    void load() {
        pathToFile = "tasks.txt";
        format = "txt";
        TxtSettingsStorage txt;
        if (txt.load(pathToFile, format)) return; // txt-разметка прочитана
        CsvSettingsStorage csv;                   // иначе пробуем csv-разметку
        csv.load(pathToFile, format);
        if (format != "txt" && format != "csv") format = "txt"; // защита от порчи
        if (pathToFile.empty()) pathToFile = "tasks.txt";
    }

public:
    static AppSettings& instance() {
        static AppSettings inst;
        return inst;
    }
    const string& getPathToFile() const { return pathToFile; }
    const string& getFormat() const { return format; }
    const IStorageFactory& factory() const { return *factory_; }

    void setPathToFile(const string& p) {
        if (!p.empty()) { pathToFile = p; save(); }
    }

    void setFormat(const string& f) {
        if (f != "txt" && f != "csv") {
            cout << "Неверный формат, допустимы только txt и csv.\n";
            return;
        }
        format = f;
        factory_ = makeStorageFactory(f); // сменилось семейство
        save();
    }

private:
    // сохранение через Abstract Factory: семейство само решает разметку
    void save() {
        factory_->createSettingsStorage()->save(pathToFile, format);
    }
};

#endif // APPSETTINGS_H
