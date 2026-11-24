#ifndef APPSETTINGS_H
#define APPSETTINGS_H
#include <string>
#include <memory>
#include "StorageFactory.h"
using namespace std;

// Singleton: настройки программы — путь к файлу задач и формат хранения.
// Формат выбран в одном месте, отсюда же берётся фабрика семейства.
class AppSettings {
public:
    static AppSettings& instance() {
        static AppSettings s;   // Meyers' Singleton
        return s;
    }

    const string& getPathToFile() const { return pathToFile; }
    const string& getFormat() const { return format; }

    void setPathToFile(const string& p) { pathToFile = p; }

    void setFormat(const string& f) {
        format = (f == "csv") ? "csv" : "txt";
        rebuildFactory();       // формат изменился — фабрика другая
    }

    // доступ к выбранному семейству фабрик
    IStorageFactory& factory() { return *factory_; }

    // сохранить/загрузить настройки через выбранное семейство
    bool saveSettings() const {
        return factory_->createSettingsStorage()->save(pathToFile, format);
    }
    bool loadSettings() {
        unique_ptr<ISettingsStorage> s = factory_->createSettingsStorage();
        string p, f;
        if (!s->load(p, f)) return false;
        pathToFile = p;
        format = (f == "csv") ? "csv" : "txt";
        return true;
    }

private:
    AppSettings() : pathToFile("tasks.txt"), format("txt") {
        rebuildFactory();
    }
    AppSettings(const AppSettings&);            // копирование запрещено
    AppSettings& operator=(const AppSettings&);

    void rebuildFactory() {
        if (format == "csv")
            factory_.reset(new CsvStorageFactory());
        else
            factory_.reset(new TxtStorageFactory());
    }

    string pathToFile;
    string format;
    unique_ptr<IStorageFactory> factory_;
};

#endif // APPSETTINGS_H
