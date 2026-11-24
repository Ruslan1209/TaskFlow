#pragma once
#include <vector>
#include <memory>
#include "Task.h"
#include "TaskFilter.h"
using namespace std;

class TaskList {
private:
    vector<unique_ptr<Task>> tasks_;   // композиция: список владеет задачами
    int nextId = 1;                    // источник уникальных номеров — только здесь
public:
    int addTask(unique_ptr<Task> task) {
        int id = nextId++;
        task->setId(id);
        tasks_.push_back(move(task));
        return id;
    }

    // правим поля существующей задачи на месте — не пересоздаём объект
    bool editTask(int id, const Task& patch) {
        Task* t = find(id);
        if (!t) return false;
        t->setTitle(patch.getTitle());
        t->setDescription(patch.getDescription());
        t->setDate(patch.getDate());
        t->setPriority(patch.getPriority());
        t->setTags(patch.getTags());
        return true;
    }

    Task* find(int id) {
        for (auto& up : tasks_)
            if (up->getId() == id) return up.get();
        return nullptr;
    }

    bool removeTask(int id) 
    {
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (tasks_[i]->getId() == id) {
                tasks_.erase(tasks_.begin() + i);
                return true;
            }
        return false;
    }

     void clear() { tasks.clear(); nextId = 1; } 

    int nextTaskId() const { return nextId; }                 // понадобится при загрузке (шаг 4)
    void bumpNextTaskId(int v) { if (v >= nextId) nextId = v + 1; }

    const vector<unique_ptr<Task>>& all() const { return tasks_; }

    vector<Task*> findBy(const TaskFilter& filter) const {
        vector<Task*> result;
        for (const auto& up : tasks_)
            if (filter.matches(*up)) result.push_back(up.get());
        return result;
    }

    vector<Task*> sortedByDate() const {
        vector<Task*> result;
        for (const auto& up : tasks_) result.push_back(up.get());
        for (size_t i = 0; i + 1 < result.size(); ++i)
            for (size_t j = 0; j + 1 < result.size() - i; ++j)
                if (result[j]->getDate() > result[j + 1]->getDate())
                    swap(result[j], result[j + 1]);
        return result;
    }
};
