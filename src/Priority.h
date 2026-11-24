#ifndef PRIORITY_H
#define PRIORITY_H
#include <string>
using namespace std;

enum class Priority { Low, Medium, High };

inline string priorityName(Priority p) {
    switch (p) {
        case Priority::High:   return "высокий";
        case Priority::Medium: return "средний";
        default:               return "низкий";
    }
}
#endif // PRIORITY_H
