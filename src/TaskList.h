#pragma once
#include <vector>
#include <memory>
#include "Task.h"
#include "TaskFilter.h"
#include "TaskSearchStrategy.h"
#include "TaskSortStrategy.h"
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
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (tasks_[i]->getId() == id) return tasks_[i].get();
        return nullptr;
    }

    bool removeTask(int id) {
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (tasks_[i]->getId() == id) {
                tasks_.erase(tasks_.begin() + i);
                return true;
            }
        return false;
    }

    // полная очистка списка перед загрузкой из файла (шаг 4)
    void clear() { tasks_.clear(); nextId = 1; }

    int nextTaskId() const { return nextId; }
    void bumpNextTaskId(int v) { if (v >= nextId) nextId = v + 1; }

    const vector<unique_ptr<Task>>& all() const { return tasks_; }

    // Strategy: критерий поиска список получает снаружи,
    // внутри списка нет ни одного if по виду критерия
    vector<Task*> findBy(const SearchStrategy& s) const {
        vector<Task*> result;
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (s.matches(*tasks_[i])) result.push_back(tasks_[i].get());
        return result;
    }

    // Strategy: порядок сортировки тоже приходит снаружи
    vector<Task*> sortedBy(const SortStrategy& s) const {
        vector<Task*> result;
        for (size_t i = 0; i < tasks_.size(); ++i) result.push_back(tasks_[i].get());
        for (size_t i = 0; i + 1 < result.size(); ++i)
            for (size_t j = 0; j + 1 < result.size() - i; ++j)
                if (s.earlier(*result[j + 1], *result[j]))
                    swap(result[j], result[j + 1]);
        return result;
    }

    vector<Task*> findBy(const TaskFilter& filter) const {
        vector<Task*> result;
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (filter.matches(*tasks_[i])) result.push_back(tasks_[i].get());
        return result;
    }

    vector<Task*> sortedByDate() const {
        vector<Task*> result;
        for (size_t i = 0; i < tasks_.size(); ++i) result.push_back(tasks_[i].get());
        for (size_t i = 0; i + 1 < result.size(); ++i)
            for (size_t j = 0; j + 1 < result.size() - i; ++j)
                if (result[j]->getDate() > result[j + 1]->getDate())
                    swap(result[j], result[j + 1]);
        return result;
    }
};
