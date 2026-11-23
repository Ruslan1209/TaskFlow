
// FileStorage.h
#pragma once
#include <string>
#include "ITaskStorage.h"
using namespace std;

// Базовый класс файловых хранилищ: реализует интерфейс
class FileStorage : public ITaskStorage {
protected:
    string filename_;
public:
    explicit FileStorage(const string& filename) : filename_(filename) {}
    virtual ~FileStorage() {}

    bool save(const vector<Task>& tasks) override {
        // TODO: общая логика записи в файл
        return false;
    }
    vector<Task> load() override {
        // TODO: общая логика чтения из файла
        return {};
    }
};
