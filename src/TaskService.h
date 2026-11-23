#ifndef TASKSERVICE_H
#define TASKSERVICE_H
#include "Task.h"
#include "TaskList.h"

class TaskService {
    TaskList* taskList_;

public:
    TaskService() : taskList_(nullptr) {}
    void setTaskList(TaskList* list) { taskList_ = list; }

    // валидация: заголовок не пуст, приоритет 1..3 (согласовано с Priority)
    bool validate(const std::string& title, int priority) const {
        if (title.empty()) return false;
        return priority >= 1 && priority <= 3;
    }

    bool addTask(const std::string& title, int priority) {
        if (!validate(title, priority)) return false;
        if (!taskList_) return false;
        Task t;
        t.setTitle(title);
        t.setPriority(static_cast<Priority>(priority));
        taskList_->addTask(t);
        return true;
    }

    bool removeTask(int id) {
        if (!taskList_) return false;
        return taskList_->removeTask(id);
    }
};

#endif
