#ifndef TASK_H
#define TASK_H
#include <string>
#include <vector>
#include "Priority.h"
using namespace std;

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
    string title;
    string description;    // описание задачи (требование ДЗ-5)
    string date;           // формат "ГГГГ-ММ-ДД"
    Priority priority;
    TaskStatus status;
    string completedDate;
    vector<string> tags;   // настоящие теги вместо поиска по title

public:
    Task() : id(0), priority(Priority::Medium), status(TaskStatus::NEW) {}

    virtual ~Task() = default;   // базовый класс — деструктор виртуальный

    // сеттеры — операции с диаграммы последовательности
    void setId(int newId)                  { id = newId; }
    void setTitle(const string& t)         { title = t; }
    void setDescription(const string& d)   { description = d; }
    void setDate(const string& d)          { date = d; }
    void setPriority(Priority p)           { priority = p; }
    void setStatus(TaskStatus s)           { status = s; }
    void setCompletedDate(const string& d) { completedDate = d; }
    void setTags(const vector<string>& t)  { tags = t; }

    int getId() const                      { return id; }
    TaskStatus getStatus() const           { return status; }
    const string& getTitle() const         { return title; }
    const string& getDescription() const   { return description; }
    const string& getDate() const          { return date; }
    Priority getPriority() const           { return priority; }
    const vector<string>& getTags() const  { return tags; }

    // ---------- события диаграммы состояний (из ДЗ-2, без изменений) ----------

    // Новая -> В работе [дата не в прошлом]; Приостановлена / Просрочена -> В работе
    bool start(const string& today) {
        if (status == TaskStatus::NEW && today > date)
            return false;                      // сторожевое условие [дата не в прошлом]
        if (status == TaskStatus::NEW ||
            status == TaskStatus::PAUSED ||
            status == TaskStatus::OVERDUE) {
            status = TaskStatus::IN_WORK;
            return true;
        }
        return false;
    }

    // В работе -> Приостановлена
    bool pause() {
        if (status != TaskStatus::IN_WORK) return false;
        status = TaskStatus::PAUSED;
        return true;
    }

    // В работе / Приостановлена / Просрочена -> Выполнена, действие setCompletedDate(today)
    bool markDone(const string& today) {
        if (status != TaskStatus::IN_WORK &&
            status != TaskStatus::PAUSED &&
            status != TaskStatus::OVERDUE)
            return false;
        status = TaskStatus::DONE;
        setCompletedDate(today);
        return true;
    }

    // Новая / Активна -> Отменена
    bool cancel() {
        if (status == TaskStatus::DONE ||
            status == TaskStatus::CANCELLED ||
            status == TaskStatus::ARCHIVED)
            return false;
        status = TaskStatus::CANCELLED;
        return true;
    }

    // Выполнена / Просрочена / Отменена -> В архиве, действие saveToStorage()
    bool archive() {
        if (status != TaskStatus::DONE &&
            status != TaskStatus::OVERDUE &&
            status != TaskStatus::CANCELLED)
            return false;
        status = TaskStatus::ARCHIVED;
        return true;
    }

    // Проверка срока: активная задача -> Просрочена [срок прошёл]
    bool check(const string& today) {
        if (status != TaskStatus::IN_WORK && status != TaskStatus::PAUSED)
            return false;
        if (today <= date)
            return false;
        status = TaskStatus::OVERDUE;
        return true;
    }

    // ---------- поиск (ДЗ-2) ----------
    bool matchesTag(const string& tag) const {
        for (const string& t : tags) if (t == tag) return true;
        return false;
    }
    bool matchesDate(const string& d) const { return date == d; }
    bool matchesPriority(Priority p) const  { return priority == p; }
};

inline string statusName(TaskStatus s) {
    switch (s) {
        case TaskStatus::NEW:      return "Новая";
        case TaskStatus::IN_WORK:  return "В работе";
        case TaskStatus::PAUSED:   return "Приостановлена";
        case TaskStatus::OVERDUE:  return "Просрочена";
        case TaskStatus::DONE:     return "Выполнена";
        case TaskStatus::CANCELLED:return "Отменена";
        default:                   return "В архиве";
    }
}
#endif // TASK_H
