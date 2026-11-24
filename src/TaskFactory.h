#ifndef TASKFACTORY_H
#define TASKFACTORY_H
#include <string>
#include <vector>
#include <memory>
#include "Task.h"
//#include "Tasks.h" 
using namespace std;

// Creator интерфейс паттерна Factory Method
class TaskCreator {
public:
    virtual ~TaskCreator() {}
    virtual string typeName() const = 0;
    virtual unique_ptr<Task> create() const = 0;
};

// ConcreteCreator для обычной задачи
class SimpleTaskCreator : public TaskCreator {
public:
    string typeName() const { return "обычная"; }
    unique_ptr<Task> create() const {
        return unique_ptr<Task>(new SimpleTask());
    }
};

// ConcreteCreator для задачи со сроком
class DeadlineTaskCreator : public TaskCreator {
public:
    string typeName() const { return "со сроком"; }
    unique_ptr<Task> create() const {
        return unique_ptr<Task>(new DeadlineTask());
    }
};

// ConcreteCreator для повторяющейся задачи
class RecurringTaskCreator : public TaskCreator {
public:
    string typeName() const { return "повторяющаяся"; }
    unique_ptr<Task> create() const {
        return unique_ptr<Task>(new RecurringTask());
    }
};

// Singleton-фабрика: реестр создателей, создаёт задачу по имени типа
class TaskFactory {
private:
    vector<unique_ptr<TaskCreator>> creators;

    TaskFactory() {
        creators.push_back(unique_ptr<TaskCreator>(new SimpleTaskCreator()));
        creators.push_back(unique_ptr<TaskCreator>(new DeadlineTaskCreator()));
        creators.push_back(unique_ptr<TaskCreator>(new RecurringTaskCreator()));
    }
    TaskFactory(const TaskFactory&) = delete;
    TaskFactory& operator=(const TaskFactory&) = delete;

public:
    static TaskFactory& instance() {
        static TaskFactory inst;
        return inst;
    }

    // возвращает пустой указатель, если типа нет в реестре
    unique_ptr<Task> create(const string& type) const {
        for (size_t i = 0; i < creators.size(); ++i)
            if (creators[i]->typeName() == type)
                return creators[i]->create();
        return unique_ptr<Task>();
    }

    vector<string> typeNames() const {
        vector<string> names;
        for (size_t i = 0; i < creators.size(); ++i)
            names.push_back(creators[i]->typeName());
        return names;
    }
};

#endif // TASKFACTORY_H
