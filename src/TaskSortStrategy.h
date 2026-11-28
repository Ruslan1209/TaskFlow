
#ifndef TASKSORTSTRATEGY_H
#define TASKSORTSTRATEGY_H
#include <string>
#include "Task.h"
using namespace std;

// Strategy: способ упорядочивания. earlier = "a должна стоять раньше b"
class SortStrategy {
public:
    virtual ~SortStrategy() {}
    virtual bool earlier(const Task& a, const Task& b) const = 0;
};

// по сроку (дате): раньше — первым
class SortByDateStrategy : public SortStrategy {
public:
    bool earlier(const Task& a, const Task& b) const {
        return a.getDate() < b.getDate();
    }
};

// по приоритету: High=1 стоит первым, потом Medium, потом Low
class SortByPriorityStrategy : public SortStrategy {
public:
    bool earlier(const Task& a, const Task& b) const {
        return a.getPriority() < b.getPriority();
    }
};

// по названию: по алфавиту
class SortByTitleStrategy : public SortStrategy {
public:
    bool earlier(const Task& a, const Task& b) const {
        return a.getTitle() < b.getTitle();
    }
};
#endif // TASKSORTSTRATEGY_H
