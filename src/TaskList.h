// TaskList.h
#pragma once
#include <vector>
#include "Task.h"
#include "TaskFilter.h"
using namespace std;

class TaskList 
{
private:
    vector<Task> tasks_;   // композиция: задачи живут внутри списка
public:
    void addTask(const Task& task) { tasks_.push_back(task); }

    bool editTask(int id, const Task& task) {
        for (Task& t : tasks_)
            if (t.getId() == id) { t = task; return true; }
        return false;
    }

    bool removeTask(int id) {
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (tasks_[i].getId() == id) {
                tasks_.erase(tasks_.begin() + i);
                return true;
            }
        return false;
    }

    vector<Task> findBy(const TaskFilter& filter) const // зависимость от TaskFilter
    {   
        vector<Task> result;
        for (const Task& t : tasks_)
            if (filter.matches(t)) result.push_back(t);
        return result;
    }

    vector<Task> sortedByDate() const 
   {
        vector<Task> result = tasks_;
        for (size_t i = 0; i + 1 < result.size(); ++i)
            for (size_t j = 0; j + 1 < result.size() - i; ++j)
                if (result[j].getDate() > result[j + 1].getDate())
                    swap(result[j], result[j + 1]);
        return result;
    }
};
