// main.cpp — подключает все классы TaskFlow
#include "src/Priority.h"
#include "src/Tag.h"
#include "src/Task.h"
#include "src/TaskFilter.h"
#include "src/TaskList.h"
#include "src/User.h"
#include "src/Reminder.h"
#include "src/ITaskStorage.h"
#include "src/FileStorage.h"
#include "src/CsvTaskStorage.h"
using namespace std;

int main() 
{
    User user("Иван");
    TaskList list;
    user.attachList(&list);              // ассоциация

    Tag study("учёба");
    Task t1(1, "Сдать лабу", "2026-11-20", Priority::HIGH);
    t1.addTag(&study);                   // агрегация
    list.addTask(t1);                    // композиция

    TaskFilter filter;
    filter.setTag("учёба");
    vector<Task> found = list.findBy(filter);

    Reminder reminder;
    reminder.setToday("2026-11-17");
    vector<Task> urgent = reminder.check(found, "2026-11-17");

    CsvTaskStorage storage("tasks.csv");
    storage.save(found);
}
