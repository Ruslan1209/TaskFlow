
#ifndef TASKCOMMAND_H
#define TASKCOMMAND_H
#include <string>
#include <vector>
#include <memory>
#include "Task.h"
#include "TaskList.h"
#include "TaskBuilder.h"
using namespace std;

// разбор строки тегов — та же логика, что и в фасаде
inline vector<string> splitTagsList(const string& line) {
    vector<string> tags;
    string part;
    istringstream iss(line);
    while (getline(iss, part, ',')) {
        size_t a = part.find_first_not_of(" \t");
        size_t b = part.find_last_not_of(" \t");
        if (a != string::npos) tags.push_back(part.substr(a, b - a + 1));
    }
    return tags;
}

// Command: одно действие над списком — выполнить и отменить
class TaskCommand {
public:
    virtual ~TaskCommand() {}
    virtual string execute() = 0;
    virtual string undo() = 0;
    virtual string name() const = 0;
};

// снимок редактируемых полей — чтобы откатить изменения
struct TaskSnapshot {
    string title, desc, date;
    Priority priority = Priority::Medium;
    Status status = Status::New;
    vector<string> tags;

    static TaskSnapshot capture(const Task* t) {
        TaskSnapshot s;
        s.title = t->getTitle();
        s.desc = t->getDescription();
        s.date = t->getDate();
        s.priority = t->getPriority();
        s.status = t->getStatus();
        s.tags = t->getTags();
        return s;
    }
    void applyTo(Task* t) const {
        t->setTitle(title);
        t->setDescription(desc);
        t->setDate(date);
        t->setPriority(priority);
        t->setStatus(status);
        t->setTags(tags);
    }
};

// добавление задачи
class AddTaskCommand : public TaskCommand {
    TaskList& list_;
    string type_, title_, desc_, date_, tagsLine_;
    Priority prio_;
    int interval_;
    int id_ = 0;
public:
    AddTaskCommand(TaskList& list, const string& type, const string& title,
                   const string& desc, Priority prio, const string& date,
                   const string& tagsLine, int interval)
        : list_(list), type_(type), title_(title), desc_(desc),
          prio_(prio), date_(date), tagsLine_(tagsLine), interval_(interval) {}

    int id() const { return id_; }
    string title() const { return title_; }

    string execute() {
        TaskBuilder b;
        b.forType(type_).title(title_).description(desc_)
         .priority(prio_).date(date_).tags(splitTagsList(tagsLine_));
        if (interval_ > 0) b.intervalDays(interval_);
        unique_ptr<Task> t = b.build();
        id_ = list_.addTask(move(t));
        return "Задача создана, номер " + to_string(id_) + ".";
    }
    string undo() {
        if (list_.removeTask(id_))
            return "Отменено добавление: задача #" + to_string(id_) +
                   " \"" + title_ + "\" удалена.";
        return "Не удалось отменить: задача #" + to_string(id_) + " не найдена.";
    }
    string name() const { return "добавление \"" + title_ + "\""; }
};

// изменение задачи
class EditTaskCommand : public TaskCommand {
    TaskList& list_;
    int id_;
    TaskSnapshot before_, after_;
public:
    EditTaskCommand(TaskList& list, int id,
                    const TaskSnapshot& before, const TaskSnapshot& after)
        : list_(list), id_(id), before_(before), after_(after) {}

    string execute() {
        Task* t = list_.find(id_);
        if (!t) return "Задача #" + to_string(id_) + " не найдена.";
        after_.applyTo(t);
        list_.notifyObservers(TaskEventType::Changed, *t);
        return "Изменения задачи #" + to_string(id_) + " применены заново.";
    }
    string undo() {
        Task* t = list_.find(id_);
        if (!t) return "Задача #" + to_string(id_) + " не найдена.";
        before_.applyTo(t);
        list_.notifyObservers(TaskEventType::Changed, *t);
        return "Изменения задачи #" + to_string(id_) + " отменены.";
    }
    string name() const { return "изменение задачи #" + to_string(id_); }
};

// удаление задачи
class RemoveTaskCommand : public TaskCommand {
    TaskList& list_;
    int id_;
    unique_ptr<Task> copy_;   // снимок задачи, чтобы вернуть её назад
public:
    RemoveTaskCommand(TaskList& list, int id, unique_ptr<Task> copy)
        : list_(list), id_(id), copy_(move(copy)) {}

    string execute() {
        if (list_.removeTask(id_))
            return "Задача #" + to_string(id_) + " удалена.";
        return "Задача #" + to_string(id_) + " не найдена.";
    }
    string undo() {
        if (!copy_) return "Не удалось вернуть задачу.";
        list_.restoreTask(copy_->clone());
        return "Отменено удаление: задача #" + to_string(id_) + " возвращена.";
    }
    string name() const { return "удаление задачи #" + to_string(id_); }
};

// история: стек выполненных команд и стек отменённых
class CommandHistory {
    vector<unique_ptr<TaskCommand>> done_, undone_;
    static const size_t LIMIT = 20;
public:
    bool canUndo() const { return !done_.empty(); }
    bool canRedo() const { return !undone_.empty(); }

    // команда уже выполнена — просто фиксируем её в истории
    void pushExecuted(unique_ptr<TaskCommand> cmd) {
        done_.push_back(move(cmd));
        if (done_.size() > LIMIT) done_.erase(done_.begin());
        undone_.clear();
    }
    // команда выполняется здесь и попадает в историю
    string run(unique_ptr<TaskCommand> cmd) {
        string msg = cmd->execute();
        pushExecuted(move(cmd));
        return msg;
    }
    string undo() {
        if (done_.empty()) return "Отменять нечего.";
        unique_ptr<TaskCommand> c = move(done_.back());
        done_.pop_back();
        string msg = c->undo();
        undone_.push_back(move(c));
        return msg;
    }
    string redo() {
        if (undone_.empty()) return "Повторять нечего.";
        unique_ptr<TaskCommand> c = move(undone_.back());
        undone_.pop_back();
        string msg = c->execute();
        done_.push_back(move(c));
        return msg;
    }
    void clear() { done_.clear(); undone_.clear(); }
};
#endif // TASKCOMMAND_H
