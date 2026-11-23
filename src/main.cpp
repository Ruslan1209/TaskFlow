#include <iostream>
#include "Priority.h"
#include "Task.h"
#include "TaskService.h"
using namespace std;

void report(const string& action, bool ok) {
    cout << (ok ? "сработал:      " : "ЗАПРЕЩЁН:  ") << action << "\n";
}

int main() {
    // 1. Новая задача, срок в будущем
    Task t1;
    t1.setId(1);
    t1.setTitle("Сдать лабу");
    t1.setDate("2026-11-25");
    t1.setPriority(Priority::HIGH);
    cout << "Задача 1 создана, статус: NEW\n";

    // 2. NEW -> IN_WORK
    report("start   (NEW -> IN_WORK)", t1.start("2026-11-21"));

    // 3. повторный start из IN_WORK — запрещён
    report("start   (IN_WORK -> IN_WORK)", t1.start("2026-11-21"));

    // 4. IN_WORK -> PAUSED
    report("pause   (IN_WORK -> PAUSED)", t1.pause());

    // 5. PAUSED -> IN_WORK
    report("start   (PAUSED -> IN_WORK)", t1.start("2026-11-21"));

    // 6. IN_WORK -> DONE
    report("markDone(IN_WORK -> DONE)", t1.markDone("2026-11-22"));

    // 7. cancel после DONE — запрещён
    report("cancel  (DONE -> CANCELLED)", t1.cancel());

    // 8. DONE -> ARCHIVED
    report("archive (DONE -> ARCHIVED)", t1.archive());

    // 9. повторный archive — запрещён
    report("archive (ARCHIVED -> ARCHIVED)", t1.archive());

    // 10. NEW с прошедшим сроком: start запрещён
    Task t2;
    t2.setId(2);
    t2.setTitle("Оплатить интернет");
    t2.setDate("2026-11-01");
    t2.setPriority(Priority::NORMAL);
    cout << "Задача 2 создана, срок уже прошёл, статус: NEW\n";
    report("start   (NEW, срок прошёл)", t2.start("2026-11-21"));

    // 11. NEW -> CANCELLED
    report("cancel  (NEW -> CANCELLED)", t2.cancel());

    // 12. CANCELLED -> ARCHIVED
    report("archive (CANCELLED -> ARCHIVED)", t2.archive());

    // 13. Просрочка: работали дольше срока, срок истёк
    Task t3;
    t3.setId(3);
    t3.setTitle("Прочитать главу");
    t3.setDate("2026-11-19");
    t3.setPriority(Priority::LOW);
    t3.start("2026-11-15");
    cout << "Задача 3 в работе, срок истёк\n";
    report("check   (IN_WORK -> OVERDUE)", t3.check("2026-11-21"));

    // 14. OVERDUE -> DONE
    report("markDone(OVERDUE -> DONE)", t3.markDone("2026-11-21"));
}
