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

#endif // TASK_H
