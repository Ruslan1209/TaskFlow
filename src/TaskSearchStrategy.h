#ifndef TASKSEARCHSTRATEGY_H
#define TASKSEARCHSTRATEGY_H
#include <string>
#include "Task.h"
using namespace std;

// Strategy: общий интерфейс критерия поиска.
// Список знает только этот интерфейс — какой критерий внутри, ему неважно.
class SearchStrategy {
public:
    virtual ~SearchStrategy() {}
    virtual bool matches(const Task& t) const = 0;
};

// поиск по тегу
class TagSearchStrategy : public SearchStrategy {
    string tag_;
public:
    explicit TagSearchStrategy(const string& tag) : tag_(tag) {}
    bool matches(const Task& t) const { return t.matchesTag(tag_); }
};

// поиск по приоритету
class PrioritySearchStrategy : public SearchStrategy {
    Priority p_;
public:
    explicit PrioritySearchStrategy(Priority p) : p_(p) {}
    bool matches(const Task& t) const { return t.matchesPriority(p_); }
};

// поиск по точной дате
class DateSearchStrategy : public SearchStrategy {
    string date_;
public:
    explicit DateSearchStrategy(const string& d) : date_(d) {}
    bool matches(const Task& t) const { return t.matchesDate(date_); }
};

// поиск по диапазону дат: пустая граница = границы нет с этой стороны
class DateRangeSearchStrategy : public SearchStrategy {
    string from_, to_;
public:
    DateRangeSearchStrategy(const string& from, const string& to)
        : from_(from), to_(to) {}
    bool matches(const Task& t) const {
        const string& d = t.getDate();
        if (!from_.empty() && d < from_) return false;
        if (!to_.empty() && d > to_) return false;
        return true;
    }
};

// поиск по тексту в названии
class TextSearchStrategy : public SearchStrategy {
    string text_;
public:
    explicit TextSearchStrategy(const string& text) : text_(text) {}
    bool matches(const Task& t) const {
        return t.getTitle().find(text_) != string::npos;
    }
};
#endif // TASKSEARCHSTRATEGY_H
