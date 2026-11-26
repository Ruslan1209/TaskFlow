#ifndef LEGACYTODOADAPTER_H
#define LEGACYTODOADAPTER_H
#include <string>
#include <vector>
#include <memory>
#include "LegacyTodo.h"
#include "TaskBuilder.h"
using namespace std;

// наш интерфейс импорта — то, что умеет использовать TaskFlow
class ITaskImporter {
public:
    virtual ~ITaskImporter() {}
    virtual vector<unique_ptr<Task>> importAll() const = 0;
};

// Adapter: оборачивает чужой LegacyTodo и выдаёт задачи нашего проекта.
// Приоритет 0-9 переводится в 1-3, время Unix — в дату ГГГГ-ММ-ДД.
class LegacyTodoAdapter : public ITaskImporter {
    LegacyTodo legacy; // «чужая» библиотека без изменений

    static Priority mapPriority(int p) {
        if (p >= 7) return Priority::High;   // 7..9 — высокие
        if (p >= 4) return Priority::Medium; // 4..6 — средние
        return Priority::Low;                // 0..3 — низкие
    }

    // секунды с 01.01.1970 -> ГГГГ-ММ-ДД (перевод дней в календарную дату)
    static string dateFromUnix(long long sec) {
        long long days = sec / 86400;
        days += 719468;
        long long era = (days >= 0 ? days : days - 146096) / 146097;
        long long doe = days - era * 146097;
        long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
        long long y = yoe + era * 400;
        long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
        long long mp = (5 * doy + 2) / 153;
        long long d = doy - (153 * mp + 2) / 5 + 1;
        long long m = (mp < 10) ? mp + 3 : mp - 9;
        if (m <= 2) y += 1;
        string s = to_string(y);
        if (s.size() < 4) s = string(4 - s.size(), '0') + s;
        string mm = to_string(m), dd = to_string(d);
        if (mm.size() < 2) mm = "0" + mm;
        if (dd.size() < 2) dd = "0" + dd;
        return s + "-" + mm + "-" + dd;
    }

public:
    vector<unique_ptr<Task>> importAll() const {
        vector<LegacyItem> items = legacy.exportAll();
        vector<unique_ptr<Task>> tasks;
        for (size_t i = 0; i < items.size(); ++i) {
            // создание только через Builder, тип — через фабрику из ДЗ-5
            tasks.push_back(TaskBuilder()
                .forType("со сроком")
                .title(items[i].text)
                .priority(mapPriority(items[i].prio))
                .date(dateFromUnix(items[i].dueUnix))
                .build());
        }
        return tasks;
    }
};

#endif // LEGACYTODOADAPTER_H
