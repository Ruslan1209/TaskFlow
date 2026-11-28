#pragma once
// LegacyFileAdapter.h
// Паттерн Adapter (шаг 4): старый формат файла
// "название | приоритет | дата | описание"
// прячем за тем же интерфейсом ITaskStorage, что и новый формат.
#ifndef LEGACYFILEADAPTER_H
#define LEGACYFILEADAPTER_H

#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "TaskStorage.h"   // здесь живёт ITaskStorage
#include "TaskBuilder.h"
using namespace std;

inline string trimSpaces(const string& s) {
    size_t a = s.find_first_not_of(" \t\r");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r");
    return s.substr(a, b - a + 1);
}

class LegacyFileAdapter : public ITaskStorage {
public:
    // в старом формате сохранять не умеем — только читаем
    bool save(const TaskList&, const string&) const { return false; }

    int load(TaskList& list, const string& path) const {
        ifstream in(path.c_str());
        if (!in.is_open()) return -1;
        int loaded = 0;
        string line;
        while (getline(in, line)) {
            if (trimSpaces(line).empty()) continue;

            istringstream iss(line);
            string title, prio, date, desc;
            if (!getline(iss, title, '|')) continue;
            if (!getline(iss, prio, '|')) continue;
            if (!getline(iss, date, '|')) continue;
            getline(iss, desc);   // описание может быть пустым

            title = trimSpaces(title);
            prio  = trimSpaces(prio);
            date  = trimSpaces(date);
            desc  = trimSpaces(desc);

            if (title.empty() || !isOldFormatDate(date)) continue;

            int p = atoi(prio.c_str());
            Priority pr = (p == 1 ? Priority::High :
                           p == 3 ? Priority::Low : Priority::Medium);

            TaskBuilder b;
            b.forType("обычная").title(title).description(desc)
             .priority(pr).date(date);
            list.addTask(b.build());
            ++loaded;
        }
        return loaded;
    }

private:
    // своя проверка даты, чтобы адаптер ни от чего другого не зависел
    static bool isOldFormatDate(const string& s) {
        if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
        for (size_t i = 0; i < s.size(); ++i) {
            if (i == 4 || i == 7) continue;
            if (s[i] < '0' || s[i] > '9') return false;
        }
        return true;
    }
};
#endif // LEGACYFILEADAPTER_H
