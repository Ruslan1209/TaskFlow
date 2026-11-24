#ifndef PRIORITY_H
#define PRIORITY_H
#include <string>
using namespace std;

enum class Priority { High = 1, Medium = 2, Low = 3 };

inline string priorityName(Priority p) {
    if (p == Priority::High) return "высокий";
    if (p == Priority::Medium) return "средний";
    return "низкий";
}

enum class Status { New, InProgress, Done };

inline string statusName(Status s) {
    if (s == Status::New) return "Новая";
    if (s == Status::InProgress) return "В работе";
    return "Готова";
}

#endif // PRIORITY_H
