#ifndef TASK_H
#define TASK_H

#include <string>
#include "Priority.h"   // Priority уже объявлен у тебя отдельным файлом

enum class TaskStatus {
    NEW,        // Новая
    IN_WORK,    // В работе
    PAUSED,     // Приостановлена
    OVERDUE,    // Просрочена
    DONE,       // Выполнена
    CANCELLED,  // Отменена
    ARCHIVED    // В архиве
};

class Task {
    int id;
    std::string title;
    std::string date;      // формат "ГГГГ-ММ-ДД"
    Priority priority;
    TaskStatus status;
    std::string completedDate;

public:
    Task() : id(0), priority(Priority::NORMAL), status(TaskStatus::NEW) {}

    // сеттеры — операции с диаграммы последовательности
    void setId(int newId)               { id = newId; }
    void setTitle(const std::string& t) { title = t; }
    void setDate(const std::string& d)  { date = d; }
    void setPriority(Priority p)        { priority = p; }
    void setStatus(TaskStatus s)        { status = s; }
    void setCompletedDate(const std::string& d) { completedDate = d; }

    TaskStatus  getStatus() const { return status; }
    std::string getTitle() const  { return title; }
    std::string getDate() const   { return date; }

    // ---------- события диаграммы состояний ----------

    bool start(const std::string& today) {
        if (status == TaskStatus::NEW && today > date)
            return false;                      // [дата не в прошлом]
        if (status == TaskStatus::NEW ||
            status == TaskStatus::PAUSED ||
            status == TaskStatus::OVERDUE) {
            status = TaskStatus::IN_WORK;
            return true;
        }
        return false;
    }

    bool pause() {
        if (status != TaskStatus::IN_WORK) return false;
        status = TaskStatus::PAUSED;
        return true;
    }

    bool markDone(const std::string& today) {
        if (status != TaskStatus::IN_WORK &&
            status != TaskStatus::PAUSED &&
            status != TaskStatus::OVERDUE)
            return false;
        status = TaskStatus::DONE;
        setCompletedDate(today);
        return true;
    }

    bool cancel() {
        if (status == TaskStatus::DONE ||
            status == TaskStatus::CANCELLED ||
            status == TaskStatus::ARCHIVED)
            return false;
        status = TaskStatus::CANCELLED;
        return true;
    }

    bool archive() {
        if (status != TaskStatus::DONE &&
            status != TaskStatus::OVERDUE &&
            status != TaskStatus::CANCELLED)
            return false;
        status = TaskStatus::ARCHIVED;
        return true;
    }

    bool check(const std::string& today) {
        if (status != TaskStatus::IN_WORK &&
            status != TaskStatus::PAUSED)
            return false;
        if (today <= date)
            return false;                      // [срок прошёл] не выполнено
        status = TaskStatus::OVERDUE;
        return true;
    }

    // ---------- методы из ДЗ-2 ----------
    bool matchesTag(const std::string& tag) const { return title.find(tag) != std::string::npos; }
    bool matchesDate(const std::string& d) const  { return date == d; }
    bool matchesPriority(Priority p) const        { return priority == p; }
};

#endif
