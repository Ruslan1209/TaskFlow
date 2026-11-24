#ifndef TASKSTORAGE_H
#define TASKSTORAGE_H
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Task.h"
#include "TaskList.h"
#include "TaskFactory.h"
using namespace std;

// безопасный текст: ';' и перевод строки внутри полей недопустимы —
// иначе строка файла развалится на части
inline string cleanText(const string& s) {
    string r = s;
    for (size_t i = 0; i < r.size(); ++i)
        if (r[i] == ';' || r[i] == '\n' || r[i] == '\r') r[i] = ' ';
    return r;
}

inline int parseIntSafe(const string& s) {
    istringstream iss(s);
    int v = 0;
    iss >> v;
    return v;
}

// один общий формат строки для обоих хранилищ:
// тип;номер;название;описание;дата;приоритет;статус;интервал;теги
inline string serializeTask(const Task* t) {
    ostringstream out;
    const RecurringTask* r = dynamic_cast<const RecurringTask*>(t);
    out << t->typeName() << ';'
        << t->getId() << ';'
        << cleanText(t->getTitle()) << ';'
        << cleanText(t->getDescription()) << ';'
        << t->getDate() << ';'
        << (int)t->getPriority() << ';'
        << (int)t->getStatus() << ';'
        << (r ? r->getInterval() : 0) << ';';
    const vector<string>& tg = t->getTags();
    for (size_t i = 0; i < tg.size(); ++i)
        out << tg[i] << (i + 1 < tg.size() ? "," : "");
    return out.str();
}

// превращает строку файла обратно в задачу и добавляет в список
inline bool applyTaskLine(TaskList& list, const string& line) {
    istringstream iss(line);
    vector<string> f;
    string cell;
    while (getline(iss, cell, ';')) f.push_back(cell);
    if (f.size() < 9) return false;
    unique_ptr<Task> t = TaskFactory::instance().create(f[0]);
    if (!t) return false;
    t->setTitle(f[2]);
    t->setDescription(f[3]);
    t->setDate(f[4]);
    int p = parseIntSafe(f[5]);
    if (p >= 1 && p <= 3)
        t->setPriority(p == 1 ? Priority::High :
                       p == 2 ? Priority::Medium : Priority::Low);
    int st = parseIntSafe(f[6]);
    t->setStatus(st == 1 ? Status::InProgress :
                 st == 2 ? Status::Done : Status::New);
    if (t->typeName() == "повторяющаяся") {
        RecurringTask* r = dynamic_cast<RecurringTask*>(t.get());
        if (r) r->setInterval(parseIntSafe(f[7]));
    }
    vector<string> tags;
    {
        string part;
        istringstream tagIss(f[8]);
        while (getline(tagIss, part, ','))
            if (!part.empty()) tags.push_back(part);
    }
    t->setTags(tags);
    list.addTask(move(t));
    return true;
}

// база хранилища: общий интерфейс save/load, конкретные классы — ниже
class ITaskStorage {
public:
    virtual ~ITaskStorage() {}
    virtual bool save(const TaskList& list, const string& path) const = 0;
    virtual int load(TaskList& list, const string& path) const = 0;
};

class TxtTaskStorage : public ITaskStorage {
public:
    bool save(const TaskList& list, const string& path) const {
        ofstream out(path.c_str());
        if (!out) return false;
        const vector<unique_ptr<Task>>& all = list.all();
        for (size_t i = 0; i < all.size(); ++i)
            out << serializeTask(all[i].get()) << "\n";
        return true;
    }
    int load(TaskList& list, const string& path) const {
        ifstream in(path.c_str());
        if (!in) return -1;
        string line;
        int count = 0;
        while (getline(in, line)) {
            if (line.empty()) continue;
            if (applyTaskLine(list, line)) ++count;
        }
        return count;
    }
};

class CsvTaskStorage : public ITaskStorage {
public:
    bool save(const TaskList& list, const string& path) const {
        ofstream out(path.c_str());
        if (!out) return false;
        out << "тип;номер;название;описание;дата;приоритет;статус;интервал;теги\n";
        const vector<unique_ptr<Task>>& all = list.all();
        for (size_t i = 0; i < all.size(); ++i)
            out << serializeTask(all[i].get()) << "\n";
        return true;
    }
    int load(TaskList& list, const string& path) const {
        ifstream in(path.c_str());
        if (!in) return -1;
        string line;
        int count = 0;
        while (getline(in, line)) {
            if (line.empty()) continue;
            if (applyTaskLine(list, line)) ++count;
        }
        return count;
    }
};

// выбор хранилища по формату из настроек
inline unique_ptr<ITaskStorage> makeStorage(const string& format) {
    if (format == "csv")
        return unique_ptr<ITaskStorage>(new CsvTaskStorage());
    return unique_ptr<ITaskStorage>(new TxtTaskStorage());
}

#endif // TASKSTORAGE_H
