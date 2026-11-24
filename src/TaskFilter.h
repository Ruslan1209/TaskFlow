#pragma once
#include <string>
#include "Task.h"
using namespace std;

class TaskFilter {
private:
    string tag_;
    string date_;
    bool prioritySet_ = false;
    Priority priority_ = Priority::Medium;
public:
    void setTag(const string& tag)      { tag_ = tag; }
    void setDate(const string& d)       { date_ = d; }
    void setPriority(Priority p)        { prioritySet_ = true; priority_ = p; }
    bool matches(const Task& t) const {
        if (!tag_.empty() && !t.matchesTag(tag_)) return false;
        if (!date_.empty() && !t.matchesDate(date_)) return false;
        if (prioritySet_ && !t.matchesPriority(priority_)) return false;
        return true;
    }
};

