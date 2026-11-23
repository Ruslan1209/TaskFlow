
// CsvTaskStorage.h
#pragma once
#include "FileStorage.h"
#include "Priority.h"
#include <fstream>
#include <sstream>
using namespace std;

// Наследник: добавляет CSV-парсинг поверх общей работы с файлом
class CsvTaskStorage : public FileStorage {
public:
    explicit CsvTaskStorage(const string& filename) : FileStorage(filename) {}

    // ---- запись одной строки CSV ----
    void writeLine(const Task& task, ostream& out) {
        out << task.getId() << ';'
            << task.getTitle() << ';'
            << task.getDate() << ';'
            << priorityToText(task) << ';'
            << statusToText(task) << ';'
            << '\n';
    }

    bool save(const vector<Task>& tasks) override {
        ofstream file(filename_);              // перезаписываем файл целиком
        if (!file.is_open()) return false;
        for (const Task& t : tasks)
            writeLine(t, file);
        return true;
    }

    vector<Task> load() override {
        vector<Task> tasks;
        ifstream file(filename_);
        if (!file.is_open()) return tasks;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string id, title, date, pr, st;
            if (!getline(ss, id, ';'))    continue;
            if (!getline(ss, title, ';')) continue;
            if (!getline(ss, date, ';'))  continue;
            if (!getline(ss, pr, ';'))    continue;
            if (!getline(ss, st, ';'))    continue;

            Task t;
            t.setId(stoi(id));
            t.setTitle(title);
            t.setDate(date);
            t.setPriority(priorityFromText(pr));
            t.setStatus(statusFromText(st));
            tasks.push_back(t);
        }
        return tasks;
    }

private:
    // ---- вспомогательные преобразования enum <-> текст ----
    string priorityToText(const Task& t) const {
        switch (t.getPriority()) {
            case Priority::Low:    return "LOW";
            case Priority::Medium: return "MEDIUM";
            case Priority::High:   return "HIGH";
        }
        return "LOW";
    }
    Priority priorityFromText(const string& s) const {
        if (s == "HIGH")   return Priority::High;
        if (s == "MEDIUM") return Priority::Medium;
        return Priority::Low;
    }
    string statusToText(const Task& t) const {
        switch (t.getStatus()) {
            case TaskStatus::NEW:       return "NEW";
            case TaskStatus::IN_WORK:   return "IN_WORK";
            case TaskStatus::PAUSED:    return "PAUSED";
            case TaskStatus::OVERDUE:   return "OVERDUE";
            case TaskStatus::DONE:      return "DONE";
            case TaskStatus::CANCELLED: return "CANCELLED";
            case TaskStatus::ARCHIVED:  return "ARCHIVED";
        }
        return "NEW";
    }
    TaskStatus statusFromText(const string& s) const {
        if (s == "IN_WORK")   return TaskStatus::IN_WORK;
        if (s == "PAUSED")    return TaskStatus::PAUSED;
        if (s == "OVERDUE")   return TaskStatus::OVERDUE;
        if (s == "DONE")      return TaskStatus::DONE;
        if (s == "CANCELLED") return TaskStatus::CANCELLED;
        if (s == "ARCHIVED")  return TaskStatus::ARCHIVED;
        return TaskStatus::NEW;
    }
};
