#ifndef TASKBUILDER_H
#define TASKBUILDER_H
#include <string>
#include <vector>
#include <stdexcept>
#include "Task.h"
#include "TaskFactory.h"
using namespace std;

// Проверка даты ГГГГ-ММ-ДД. Живёт здесь, чтобы и строитель, и поиск
// в фасаде проверяли дату одним и тем же правилом
inline bool isValidDate(const string& d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (int i = 0; i < 10; ++i)
        if (i != 4 && i != 7 && (d[i] < '0' || d[i] > '9')) return false;
    int month = stoi(d.substr(5, 2));
    int day = stoi(d.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

// Builder: единственная точка сборки задачи. Поля задаются цепочкой
// вызовов, build() проверяет данные и только потом создаёт объект.
// Выбор типа build() делегирует фабрике из ДЗ-5, поэтому задачу нельзя
// создать ни в обход строителя, ни в обход создателей (Factory Method).
class TaskBuilder {
    string type_;
    string title_, description_, date_;
    Priority priority_ = Priority::Medium;
    bool prioritySet_ = false;
    vector<string> tags_;
    int intervalDays_ = -1;
public:
    TaskBuilder& forType(const string& t) { type_ = t; return *this; }
    TaskBuilder& title(const string& s) { title_ = s; return *this; }
    TaskBuilder& description(const string& s) { description_ = s; return *this; }
    TaskBuilder& date(const string& s) { date_ = s; return *this; }
    TaskBuilder& priority(Priority p) { priority_ = p; prioritySet_ = true; return *this; }
    TaskBuilder& priority(int p) {
        if (p == 1) priority_ = Priority::High;
        else if (p == 2) priority_ = Priority::Medium;
        else if (p == 3) priority_ = Priority::Low;
        else throw invalid_argument("Приоритет должен быть числом от 1 до 3 (введено: " + to_string(p) + ").");
        prioritySet_ = true;
        return *this;
    }
    TaskBuilder& tags(const vector<string>& t) { tags_ = t; return *this; }
    TaskBuilder& intervalDays(int n) { intervalDays_ = n; return *this; }

    // при любой ошибке данных бросает invalid_argument — меню ловит
    // исключение и показывает понятное сообщение
    unique_ptr<Task> build() const {
        string type = type_.empty() ? "обычная" : type_;
        if (title_.empty())
            throw invalid_argument("Название не может быть пустым.");
        if (!prioritySet_)
            throw invalid_argument("Приоритет не указан (1-высокий, 2-средний, 3-низкий).");
        if (!date_.empty() && !isValidDate(date_))
            throw invalid_argument("Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15.");
        if (type == "повторяющаяся" && intervalDays_ >= 0 && intervalDays_ < 1)
            throw invalid_argument("Интервал повторения должен быть не меньше 1 дня.");
        unique_ptr<Task> t = TaskFactory::instance().create(type);
        if (!t)
            throw invalid_argument("Нет такого типа задачи: " + type + ".");
        t->setTitle(title_);
        t->setDescription(description_);
        t->setDate(date_);
        t->setPriority(priority_);
        t->setTags(tags_);
        if (RecurringTask* r = dynamic_cast<RecurringTask*>(t.get()))
            if (intervalDays_ > 0) r->setInterval(intervalDays_);
        return t;
    }
};

#endif // TASKBUILDER_H
