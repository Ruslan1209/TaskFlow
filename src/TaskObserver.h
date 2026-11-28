#ifndef TASKOBSERVER_H
#define TASKOBSERVER_H
#include "Task.h"
using namespace std;

// Observer: вид события, по которому оповещают подписчиков
enum class TaskEventType { Added, Changed, Removed };

// интерфейс подписчика: список знает только его, что внутри — неважно
class TaskObserver {
public:
    virtual ~TaskObserver() {}
    virtual void onTaskEvent(TaskEventType type, const Task& task) = 0;
};
#endif // TASKOBSERVER_H
