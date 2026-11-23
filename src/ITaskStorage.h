// ITaskStorage.h
#pragma once
#include <vector>
#include "Task.h"
using namespace std;

// Интерфейс: только чисто виртуальные методы
class ITaskStorage {
public:
    virtual ~ITaskStorage() {}
    virtual bool save(const vector<Task>& tasks) = 0;
    virtual vector<Task> load() = 0;
};
