#include <iostream>
#include "Task.h"

std::string statusName(TaskStatus s) {
    switch (s) {
        case TaskStatus::NEW:       return "Новая";
        case TaskStatus::IN_WORK:   return "В работе";
        case TaskStatus::PAUSED:    return "Приостановлена";
        case TaskStatus::OVERDUE:   return "Просрочена";
        case TaskStatus::DONE:      return "Выполнена";
        case TaskStatus::CANCELLED: return "Отменена";
        case TaskStatus::ARCHIVED:  return "В архиве";
    }
    return "?";
}

void show(const Task& t, const std::string& label, bool ok) {
    std::cout << label << (ok ? " -> сработал: " : " -> ЗАПРЕЩЁН: ")
              << statusName(t.getStatus()) << "\n";
}

int main() {
    std::string today = "2026-11-21";

    std::cout << "--- Допустимые переходы ---\n";
    Task t;
    t.setId(1);
    t.setTitle("Сделать ДЗ-3");
    t.setDate("2026-11-25");
    t.setPriority(Priority::HIGH);
    show(t, "1) start()      для Новой",              t.start(today));           // Новая -> В работе
    show(t, "2) pause()      для задачи в работе",    t.pause());                // В работе -> Приостановлена
    show(t, "3) start()      для Приостановленной",   t.start(today));           // Приостановлена -> В работе
    show(t, "4) check(\"2026-12-01\") — срок прошёл", t.check("2026-12-01"));    // -> Просрочена
    show(t, "5) markDone()   для Просроченной",       t.markDone(today));        // Просрочена -> Выполнена
    show(t, "6) archive()    для Выполненной",        t.archive());              // Выполнена -> В архиве

    std::cout << "\n--- Проверка запрещённых переходов ---\n";
    Task t2;
    t2.setTitle("Отчёт");
    t2.setDate("2026-12-01");
    show(t2, "7) pause()   для Новой (запрещено)",            t2.pause());
    t2.start(today);
    t2.markDone(today);
    show(t2, "8) cancel()  для Выполненной (запрещено)",      t2.cancel());
    show(t2, "9) archive() для Выполненной (допустимо)",      t2.archive());
    show(t2, "10) start()  для Архивной (запрещено)",         t2.start(today));

    return 0;
}
