#ifndef TASK_H
#define TASK_H
#include <string>
#include <vector>
#include "Priority.h"
using namespace std;

class Task {
    int id;
    string title, description, date;
    Priority priority;
    Status status;
    vector<string> tags;
public:
    Task() : id(0), priority(Priority::Medium), status(Status::New) {}
    virtual ~Task() {}
    int getId() const { return id; }
    void setId(int v) { id = v; }
    const string& getTitle() const { return title; }
    void setTitle(const string& s) { title = s; }
    const string& getDescription() const { return description; }
    void setDescription(const string& s) { description = s; }
    const string& getDate() const { return date; }
    void setDate(const string& s) { date = s; }
    Priority getPriority() const { return priority; }
    void setPriority(Priority p) { priority = p; }
    Status getStatus() const { return status; }
    void setStatus(Status s) { status = s; }
    const vector<string>& getTags() const { return tags; }
    void setTags(const vector<string>& t) { tags = t; }
    // для Factory Method: каждый тип задачи представляется по-своему
    virtual string typeName() const { return "обычная"; }
    virtual string extraInfo() const { return ""; }
    // для TaskFilter: проверка совпадений
    bool matchesTag(const string& tag) const {
        for (size_t i = 0; i < tags.size(); ++i)
            if (tags[i] == tag) return true;
        return false;
    }
    bool matchesDate(const string& d) const { return date == d; }
    bool matchesPriority(Priority p) const { return priority == p; }
};
class SimpleTask : public Task {
public:
    SimpleTask() {}
    string typeName() const { return "обычная"; }
    string extraInfo() const { return ""; }
};

class DeadlineTask : public Task {
    string deadline_;
public:
    DeadlineTask() {}
    void setDeadline(const string& d) { deadline_ = d; }
    string getDeadline() const { return deadline_; }
    string typeName() const { return "со сроком"; }
    string extraInfo() const { return deadline_; }
};

class RecurringTask : public Task {
    int intervalDays_;
public:
    RecurringTask() : intervalDays_(1) {}
    int getInterval() const { return intervalDays_; }
    void setInterval(int n) { intervalDays_ = n; }
    string typeName() const { return "повторяющаяся"; }
    string extraInfo() const { return "каждые " + to_string(intervalDays_) + " дн."; }
};
#endif // TASK_H
