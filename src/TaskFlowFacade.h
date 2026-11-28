#pragma once
#ifndef TASKFLOWFACADE_H
#define TASKFLOWFACADE_H
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include "TaskList.h"
#include "TaskBuilder.h"
#include "TaskTemplates.h"
#include "AppSettings.h"
#include "AppConfig.h"
#include "LegacyTodoAdapter.h"
#include "TaskSearchStrategy.h"
#include "TaskSortStrategy.h"
#include "TaskObservers.h"
#include "TaskCommand.h"
#include "LegacyFileAdapter.h"
using namespace std;

// Facade: единственная дверь меню во внутренние подсистемы.
// Теперь каждое изменение проходит через Command и попадает в историю.
class TaskFlowFacade {
    TaskList list_;
    AutoSaveObserver* autoSave_;
    DeadlineObserver* deadline_;
    CommandHistory history_;
    bool autoSaveOn_;

    static vector<string> splitTags(const string& line) {
        return splitTagsList(line);
    }

    static string taskText(const Task* t) {
        ostringstream os;
        os << "#" << t->getId() << " [" << statusName(t->getStatus()) << "] "
            << t->getTitle()
            << " | тип: " << t->typeName()
            << " | приоритет: " << priorityName(t->getPriority())
            << " | срок: " << t->getDate();
        if (!t->getDescription().empty()) os << " | " << t->getDescription();
        const vector<string>& tg = t->getTags();
        if (!tg.empty()) {
            os << " | теги: ";
            for (size_t i = 0; i < tg.size(); ++i)
                os << tg[i] << (i + 1 < tg.size() ? ", " : "");
        }
        if (!t->extraInfo().empty()) os << " | " << t->extraInfo();
        return os.str();
    }

public:
    TaskFlowFacade() {
        AppSettings::instance().loadSettings();
        autoSave_ = new AutoSaveObserver(list_);
        deadline_ = new DeadlineObserver();
        list_.subscribe(deadline_);
        autoSaveOn_ = AppConfig::instance().getAutoSave();
        if (autoSaveOn_) list_.subscribe(autoSave_);
    }

    ~TaskFlowFacade() {
        if (autoSaveOn_) list_.unsubscribe(autoSave_);
        list_.unsubscribe(deadline_);
        delete autoSave_;
        delete deadline_;
    }

    // --- пункт 1: создание (Builder + Factory Method) — теперь через Command ---
    vector<string> taskTypes() const { return TaskFactory::instance().typeNames(); }

    int addTask(const string& type, const string& title, const string& desc,
        Priority prio, const string& date, const string& tagsLine,
        int intervalDays) {
        unique_ptr<AddTaskCommand> cmd(
            new AddTaskCommand(list_, type, title, desc, prio,
                date, tagsLine, intervalDays));
        AddTaskCommand* raw = cmd.get();
        history_.run(move(cmd));   // если данные плохие — Builder бросит исключение до записи
        return raw->id();
    }

    // --- пункт 2 ---
    bool hasTasks() const { return !list_.all().empty(); }

    vector<string> sortNames() const {
        vector<string> v;
        v.push_back("без сортировки (как добавлены)");
        v.push_back("по сроку");
        v.push_back("по приоритету");
        v.push_back("по названию");
        return v;
    }

    vector<string> allText(int sortMode) const {
        vector<string> out;
        vector<Task*> ordered;
        if (sortMode == 1) {
            SortByDateStrategy s;
            ordered = list_.sortedBy(s);
        }
        else if (sortMode == 2) {
            SortByPriorityStrategy s;
            ordered = list_.sortedBy(s);
        }
        else if (sortMode == 3) {
            SortByTitleStrategy s;
            ordered = list_.sortedBy(s);
        }
        else {
            const vector<unique_ptr<Task>>& all = list_.all();
            for (size_t i = 0; i < all.size(); ++i)
                ordered.push_back(all[i].get());
        }
        for (size_t i = 0; i < ordered.size(); ++i)
            out.push_back(taskText(ordered[i]));
        return out;
    }

    // --- пункт 3: редактирование — тоже Command ---
    bool hasTask(int id) { return list_.find(id) != nullptr; }
    string typeNameOf(int id) {
        Task* t = list_.find(id);
        return t ? t->typeName() : "";
    }
    string fieldOf(int id, const string& field) {
        Task* t = list_.find(id);
        if (!t) return "";
        if (field == "title") return t->getTitle();
        if (field == "description") return t->getDescription();
        if (field == "priority") return priorityName(t->getPriority());
        if (field == "status") return statusName(t->getStatus());
        if (field == "date") return t->getDate();
        return "";
    }

    string editTask(int id, const string& title, const string& desc,
        const string& prio, const string& status,
        const string& date, const string& tagsLine) {
        Task* t = list_.find(id);
        if (!t) return "Задача с номером " + to_string(id) + " не найдена.";
        TaskSnapshot before = TaskSnapshot::capture(t);
        TaskSnapshot after = before;
        if (!title.empty()) after.title = title;
        if (!desc.empty()) after.desc = desc;
        if (!prio.empty()) {
            if (prio == "1") after.priority = Priority::High;
            else if (prio == "2") after.priority = Priority::Medium;
            else if (prio == "3") after.priority = Priority::Low;
            else return "Приоритет не понял, оставляю старый.";
        }
        if (!status.empty()) {
            if (status == "1") after.status = Status::New;
            else if (status == "2") after.status = Status::InProgress;
            else if (status == "3") after.status = Status::Done;
            else return "Статус не понял, оставляю старый.";
        }
        if (!date.empty()) {
            if (!isValidDate(date))
                return "Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15.";
            after.date = date;
        }
        if (!tagsLine.empty()) after.tags = splitTags(tagsLine);
        // применяем через команду: execute + фиксация в истории
        unique_ptr<EditTaskCommand> cmd(
            new EditTaskCommand(list_, id, before, after));
        cmd->execute();
        history_.pushExecuted(move(cmd));
        return "Задача обновлена.";
    }

    // --- пункт 4: удаление — Command со снимком задачи ---
    string removeTask(int id) {
        Task* t = list_.find(id);
        if (!t) return "Задача с номером " + to_string(id) + " не найдена.";
        unique_ptr<RemoveTaskCommand> cmd(
            new RemoveTaskCommand(list_, id, t->clone()));
        return history_.run(move(cmd));
    }

    // --- пункты 5 и 6 ---
    string saveAll() {
        AppSettings& s = AppSettings::instance();
        unique_ptr<ITaskStorage> st = s.factory().createTaskStorage();
        if (st->save(list_, s.getPathToFile()))
            return "Сохранено в " + s.getPathToFile() + " (формат " + s.getFormat() + ").";
        return "Не удалось открыть файл для записи: " + s.getPathToFile();
    }

    string loadAll() {
        AppSettings& s = AppSettings::instance();
        unique_ptr<ITaskStorage> st = s.factory().createTaskStorage();
        list_.clear();
        history_.clear();   // после загрузки история прошлого списка не имеет смысла
        int n = st->load(list_, s.getPathToFile());
        if (n < 0) return "Не удалось открыть файл: " + s.getPathToFile();
        return "Загружено задач: " + to_string(n) + ". Номера пересчитаны заново.";
    }

    // --- пункт 13: Adapter — загрузка старого формата ---
    string loadLegacy() {
        list_.clear();
        history_.clear();
        LegacyFileAdapter adapter;
        int n = adapter.load(list_, "todo_old.txt");
        if (n < 0) return "Не удалось открыть файл: todo_old.txt";
        return "Загружено из старого формата задач: " + to_string(n) + ".";
    }

    // --- пункт 7 ---
    string settingsText() const {
        AppSettings& s = AppSettings::instance();
        return "Настройки. Текущий формат: " + s.getFormat() +
            " | путь к файлу: " + s.getPathToFile();
    }
    string setFormat(const string& f) {
        if (f != "txt" && f != "csv")
            return "Неверный формат, допустимы только txt и csv. Настройка не изменена.";
        AppSettings::instance().setFormat(f);
        AppSettings::instance().saveSettings();
        return "Формат изменён на " + f + ".";
    }
    string setPath(const string& p) {
        if (p.empty()) return "Путь не может быть пустым.";
        AppSettings::instance().setPathToFile(p);
        AppSettings::instance().saveSettings();
        return "Путь изменён.";
    }

    // --- пункт 14: Singleton AppConfig — автосохранение вкл/выкл ---
    string autoSaveText() const {
        AppConfig& c = AppConfig::instance();
        return string("Автосохранение сейчас: ") +
            (c.getAutoSave() ? "включено" : "выключено") +
            ". Введите 1 — включить, 0 — выключить.";
    }
    string setAutoSave(bool on) {
        AppConfig& c = AppConfig::instance();
        if (c.getAutoSave() == on)
            return on ? "Автосохранение уже включено." :
                        "Автосохранение уже выключено.";
        c.setAutoSave(on);
        autoSaveOn_ = on;
        if (on) list_.subscribe(autoSave_);
        else    list_.unsubscribe(autoSave_);
        return on ? "Автосохранение включено." : "Автосохранение выключено.";
    }

    // --- пункт 8 ---
    vector<string> searchCriteria() const {
        vector<string> v;
        v.push_back("по тегу");
        v.push_back("по приоритету");
        v.push_back("по дате");
        v.push_back("по диапазону дат");
        v.push_back("по тексту в названии");
        return v;
    }

    vector<string> search(int criterion, const string& value, const string& value2) {
        vector<string> out;
        unique_ptr<SearchStrategy> s;
        if (criterion == 1) {
            if (value.empty()) { out.push_back("Тег не задан."); return out; }
            s.reset(new TagSearchStrategy(value));
        }
        else if (criterion == 2) {
            if (value == "1") s.reset(new PrioritySearchStrategy(Priority::High));
            else if (value == "2") s.reset(new PrioritySearchStrategy(Priority::Medium));
            else if (value == "3") s.reset(new PrioritySearchStrategy(Priority::Low));
            else { out.push_back("Приоритет не понял, введите 1, 2 или 3."); return out; }
        }
        else if (criterion == 3) {
            if (!isValidDate(value))
            {
                out.push_back("Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15."); return out;
            }
            s.reset(new DateSearchStrategy(value));
        }
        else if (criterion == 4) {
            if (value.empty() && value2.empty())
            {
                out.push_back("Задайте хотя бы одну границу диапазона."); return out;
            }
            if (!value.empty() && !isValidDate(value))
            {
                out.push_back("Нижняя дата должна быть ГГГГ-ММ-ДД."); return out;
            }
            if (!value2.empty() && !isValidDate(value2))
            {
                out.push_back("Верхняя дата должна быть ГГГГ-ММ-ДД."); return out;
            }
            s.reset(new DateRangeSearchStrategy(value, value2));
        }
        else {
            if (value.empty()) { out.push_back("Текст не задан."); return out; }
            s.reset(new TextSearchStrategy(value));
        }
        vector<Task*> found = list_.findBy(*s);
        if (found.empty()) { out.push_back("Ничего не найдено."); return out; }
        out.push_back("Найдено задач: " + to_string(found.size()));
        for (size_t i = 0; i < found.size(); ++i) out.push_back(taskText(found[i]));
        return out;
    }

    // --- пункт 9: шаблоны (Prototype) тоже попадают в историю через addTask ---
    vector<string> templateTexts() const {
        vector<string> names = TaskTemplates::instance().names();
        vector<string> out;
        for (size_t i = 0; i < names.size(); ++i) {
            unique_ptr<Task> copy = TaskTemplates::instance().createFrom(i);
            out.push_back(names[i] + " — " + taskText(copy.get()));
        }
        return out;
    }

    int addFromTemplate(size_t idx, const string& title) {
        if (title.empty())
            throw invalid_argument("Название не может быть пустым.");
        unique_ptr<Task> t = TaskTemplates::instance().createFrom(idx);
        if (!t) throw invalid_argument("Нет такого шаблона.");
        t->setTitle(title);
        return list_.addTask(move(t));
    }

    // --- пункт 10: импорт (Adapter) ---
    vector<string> importLegacy() {
        LegacyTodoAdapter adapter;
        vector<unique_ptr<Task>> imported = adapter.importAll();
        vector<string> out;
        for (size_t i = 0; i < imported.size(); ++i) {
            int id = list_.addTask(move(imported[i]));
            out.push_back(taskText(list_.find(id)));
        }
        return out;
    }

    // --- пункты 11 и 12: Отменить и Повторить ---
    string undo() { return history_.undo(); }
    string redo() { return history_.redo(); }
    bool canUndo() const { return history_.canUndo(); }
    bool canRedo() const { return history_.canRedo(); }
};

#endif // TASKFLOWFACADE_H
