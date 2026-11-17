// CsvTaskStorage.h
#pragma once
#include "FileStorage.h"
using namespace std;

// Наследник: добавляет CSV-парсинг поверх общей работы с файлом
class CsvTaskStorage : public FileStorage {
public:
    explicit CsvTaskStorage(const string& filename) : FileStorage(filename) {}

    bool save(const vector<Task>& tasks) override {
        // TODO: записать задачи в формате CSV
        return false;
    }
    vector<Task> load() override {
        // TODO: прочитать задачи из CSV
        return {};
    }
};
