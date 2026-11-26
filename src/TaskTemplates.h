#ifndef TASKTEMPLATES_H
#define TASKTEMPLATES_H
#include <string>
#include <vector>
#include <memory>
#include "Task.h"
using namespace std;

// Реестр шаблонов (Prototype): хранит готовые задачи-образцы и выдаёт
// их копии через clone(). Копия независима от оригинала: изменение тегов
// у копии не меняет шаблон.
class TaskTemplates {
    vector<pair<string, unique_ptr<Task>>> protos;

    TaskTemplates() {
        unique_ptr<RecurringTask> report(new RecurringTask());
        report->setTitle("Еженедельный отчёт");
        report->setDescription("Собрать итоги недели и отправить руководителю");
        report->setPriority(Priority::Medium);
        report->setInterval(7);
        report->setTags({"работа", "отчёт"});
        protos.push_back(pair<string, unique_ptr<Task>>("Еженедельный отчёт", move(report)));

        unique_ptr<DeadlineTask> bill(new DeadlineTask());
        bill->setTitle("Оплатить счёт");
        bill->setDescription("Оплатить до конца месяца, иначе пеня");
        bill->setPriority(Priority::High);
        bill->setDate("2026-12-01");
        bill->setTags({"дом", "финансы"});
        protos.push_back(pair<string, unique_ptr<Task>>("Оплатить счёт", move(bill)));
    }
    TaskTemplates(const TaskTemplates&);            // копирование запрещено
    TaskTemplates& operator=(const TaskTemplates&);

public:
    static TaskTemplates& instance() {
        static TaskTemplates inst;
        return inst;
    }
    vector<string> names() const {
        vector<string> n;
        for (size_t i = 0; i < protos.size(); ++i) n.push_back(protos[i].first);
        return n;
    }
    // глубокая копия шаблона: clone() вызывается у конкретного класса
    unique_ptr<Task> createFrom(size_t idx) const {
        if (idx >= protos.size()) return unique_ptr<Task>();
        return protos[idx].second->clone();
    }
};

#endif // TASKTEMPLATES_H
