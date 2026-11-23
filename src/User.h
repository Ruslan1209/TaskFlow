// User.h
#pragma once
#include <string>
#include "TaskList.h"
using namespace std;

class User {
private:
    string name_;
    TaskList* taskList_;   // ассоциация: указатель на чужой список
public:
    User() : name_(""), taskList_(nullptr) {}
    explicit User(const string& name) : name_(name), taskList_(nullptr) {}

    string getName() const { return name_; }
    void attachList(TaskList* list) { taskList_ = list; }
    TaskList* getTaskList() const { return taskList_; }
};
