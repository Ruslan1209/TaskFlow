#ifndef STORAGEFACTORY_H
#define STORAGEFACTORY_H
#include <string>
#include <memory>
#include "TaskStorage.h"
#include "SettingsStorage.h"
using namespace std;

// Abstract Factory: одно семейство — хранилище задач,
// второе — хранилище настроек. Фабрика создаёт обоих.
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

// старое удобное имя сохранено: выбор хранилища по строке формата,
// чтобы main.cpp не пришлось переписывать
inline unique_ptr<ITaskStorage> makeStorage(const string& format) {
    if (format == "csv") {
        CsvStorageFactory f;
        return f.createTaskStorage();
    }
    TxtStorageFactory f;
    return f.createTaskStorage();
    }

#endif // STORAGEFACTORY_H
