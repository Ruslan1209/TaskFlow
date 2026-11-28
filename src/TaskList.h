#pragma once
#include <vector>
#include <memory>
#include "Task.h"
#include "TaskFilter.h"
#include "TaskObserver.h"
#include "TaskSearchStrategy.h"
#include "TaskSortStrategy.h"
using namespace std;

class TaskList {
private:
    vector<unique_ptr<Task>> tasks_;
    vector<TaskObserver*> observers_;
    int nextId = 1;
public:
    // --- Observer: подписка, отписка, оповещение ---
    void subscribe(TaskObserver* o) { observers_.push_back(o); }
    void unsubscribe(TaskObserver* o) {
        for (size_t i = 0; i < observers_.size(); ++i)
            if (observers_[i] == o) {
                observers_.erase(observers_.begin() + i);
                return;
            }
    }
    void notifyObservers(TaskEventType type, const Task& t) {
        for (size_t i = 0; i < observers_.size(); ++i)
            observers_[i]->onTaskEvent(type, t);
    }

    int addTask(unique_ptr<Task> task) {
        int id = nextId++;
        task->setId(id);
        tasks_.push_back(move(task));
        notifyObservers(TaskEventType::Added, *tasks_.back());
        return id;
    }

    // возврат задачи с прежним номером (Command: отмена удаления)
    void restoreTask(unique_ptr<Task> task) {
        if (task->getId() >= nextId) nextId = task->getId() + 1;
        tasks_.push_back(move(task));
        notifyObservers(TaskEventType::Added, *tasks_.back());
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
                unique_ptr<Task> copy = tasks_[i]->clone();
                tasks_.erase(tasks_.begin() + i);
                notifyObservers(TaskEventType::Removed, *copy);
                return true;
            }
        return false;
    }

    void clear() { tasks_.clear(); nextId = 1; }

    int nextTaskId() const { return nextId; }
    void bumpNextTaskId(int v) { if (v >= nextId) nextId = v + 1; }

    const vector<unique_ptr<Task>>& all() const { return tasks_; }

    vector<Task*> findBy(const SearchStrategy& s) const {
        vector<Task*> result;
        for (size_t i = 0; i < tasks_.size(); ++i)
            if (s.matches(*tasks_[i])) result.push_back(tasks_[i].get());
        return result;
    }

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
