
// Reminder.h
#pragma once
#include <string>
#include <vector>
#include "Task.h"
using namespace std;

class Reminder {
private:
    string today_;   // формат YYYY-MM-DD
public:
    Reminder() : today_("") {}
    void setToday(const string& date) { today_ = date; }

    vector<Task> check(const vector<Task>& tasks, const string& today) const {
        vector<Task> result;
        for (const Task& t : tasks)
            if (t.getDate() <= today) result.push_back(t);
        return result;
    }
};
