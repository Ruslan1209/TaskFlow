#ifndef TASKOBSERVERS_H
#define TASKOBSERVERS_H
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <memory>
#include "TaskObserver.h"
#include "TaskList.h"
#include "AppSettings.h"
#include "TaskStorage.h"
using namespace std;

// Подписчик 1: автосохранение после каждого изменения списка.
// Механизм тот же, что у пункта 5 меню: фабрика из настроек + текущий путь.
class AutoSaveObserver : public TaskObserver {
    TaskList& list_;
public:
    explicit AutoSaveObserver(TaskList& list) : list_(list) {}
    void onTaskEvent(TaskEventType type, const Task& task) {
        AppSettings& s = AppSettings::instance();
        unique_ptr<ITaskStorage> st = s.factory().createTaskStorage();
        if (st->save(list_, s.getPathToFile()))
            cout << "(автосохранение: " << s.getPathToFile() << ")\n";
        else
            cout << "(автосохранение не удалось: " << s.getPathToFile() << ")\n";
    }
};

// Подписчик 2: проверка сроков. Сообщение, если срок сегодня или уже прошёл.
class DeadlineObserver : public TaskObserver {
    // сегодняшняя дата в формате ГГГГ-ММ-ДД (сравнение строк работает,
    // потому что формат ISO: сначала год)
    static string todayText() {
        time_t now = time(0);
        tm lt;
#ifdef _WIN32
        localtime_s(&lt, &now);
#else
        localtime_r(&now, &lt);
#endif
        ostringstream os;
        os << (lt.tm_year + 1900) << '-'
           << setw(2) << setfill('0') << (lt.tm_mon + 1) << '-'
           << setw(2) << setfill('0') << lt.tm_mday;
        return os.str();
    }
public:
    void onTaskEvent(TaskEventType type, const Task& task) {
        const string& d = task.getDate();
        if (d.empty()) return;
        string today = todayText();
        if (d <= today) {
            cout << "Внимание: у задачи #" << task.getId()
                 << " \"" << task.getTitle() << "\" срок "
                 << (d == today ? "сегодня" : "уже прошёл")
                 << " (" << d << ").\n";
        }
    }
};
#endif // TASKOBSERVERS_H
