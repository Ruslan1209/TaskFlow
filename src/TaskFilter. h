// TaskFilter.h
#pragma once
#include <string>
#include "Priority.h"
#include "Task.h"
using namespace std;

class TaskFilter {
private:
    string tag_;
    string date_;
    Priority priority_;
public:
    TaskFilter() : priority_(Priority::NORMAL) {}

    void setTag(const string& tag) { tag_ = tag; }
    void setDate(const string& date) { date_ = date; }
    void setPriority(Priority p) { priority_ = p; }

    bool matches(const Task& task) const {   // зависимость: Task только в параметре
        if (!tag_.empty() && !task.matchesTag(tag_)) return false;
        if (!date_.empty() && !task.matchesDate(date_)) return false;
        if (!task.matchesPriority(priority_)) return false;
        return true;
    }
};
