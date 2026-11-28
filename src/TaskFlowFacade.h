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
#include "LegacyTodoAdapter.h"
#include "TaskSearchStrategy.h"
#include "TaskSortStrategy.h"
using namespace std;

// Facade: единственная дверь меню во внутренние подсистемы.
// В main.cpp подключается только этот заголовок — ни списка,
// ни хранилища, ни фабрик меню не видит.
class TaskFlowFacade {
    TaskList list_;

    // разбор строки тегов "a, b, c" -> вектор без лишних пробелов
    static vector<string> splitTags(const string& line) {
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

    // единый вид строки задачи (бывший printTask из main)
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
    TaskFlowFacade() { AppSettings::instance().loadSettings(); }

    // --- пункт 1: создание задачи (Builder + Factory Method) ---
    vector<string> taskTypes() const { return TaskFactory::instance().typeNames(); }

    int addTask(const string& type, const string& title, const string& desc,
                Priority prio, const string& date, const string& tagsLine,
                int intervalDays) {
        TaskBuilder b;
        b.forType(type).title(title).description(desc)
         .priority(prio).date(date).tags(splitTags(tagsLine));
        if (intervalDays > 0) b.intervalDays(intervalDays);
        unique_ptr<Task> t = b.build(); // ошибка данных -> исключение, ловит меню
        return list_.addTask(move(t));
    }

    // --- пункт 2: показ всех задач, сортировка выбирается в меню (Strategy) ---
    bool hasTasks() const { return !list_.all().empty(); }

    vector<string> sortNames() const {
        vector<string> v;
        v.push_back("без сортировки (как добавлены)");
        v.push_back("по сроку");
        v.push_back("по приоритету");
        v.push_back("по названию");
        return v;
    }

    // sortMode: 0 — как добавлены, 1..3 — стратегии сортировки
    vector<string> allText(int sortMode) const {
        vector<string> out;
        vector<Task*> ordered;
        if (sortMode == 1) {
            SortByDateStrategy s;
            ordered = list_.sortedBy(s);
        } else if (sortMode == 2) {
            SortByPriorityStrategy s;
            ordered = list_.sortedBy(s);
        } else if (sortMode == 3) {
            SortByTitleStrategy s;
            ordered = list_.sortedBy(s);
        } else {
            const vector<unique_ptr<Task>>& all = list_.all();
            for (size_t i = 0; i < all.size(); ++i)
                ordered.push_back(all[i].get());
        }
        for (size_t i = 0; i < ordered.size(); ++i)
            out.push_back(taskText(ordered[i]));
        return out;
    }

    // --- пункт 3: редактирование. Пустая строка = оставить прежнее ---
    bool hasTask(int id) { return list_.find(id) != nullptr; }
    string typeNameOf(int id) {
        Task* t = list_.find(id);
        return t ? t->typeName() : "";
    }
    // старое значение поля для подсказки "[текущее]" в меню
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
        if (!title.empty()) t->setTitle(title);
        if (!desc.empty()) t->setDescription(desc);
        if (!prio.empty()) {
            if (prio == "1") t->setPriority(Priority::High);
            else if (prio == "2") t->setPriority(Priority::Medium);
            else if (prio == "3") t->setPriority(Priority::Low);
            else return "Приоритет не понял, оставляю старый.";
        }
        if (!status.empty()) {
            if (status == "1") t->setStatus(Status::New);
            else if (status == "2") t->setStatus(Status::InProgress);
            else if (status == "3") t->setStatus(Status::Done);
            else return "Статус не понял, оставляю старый.";
        }
        if (!date.empty()) {
            if (!isValidDate(date))
                return "Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15.";
            t->setDate(date);
        }
        if (!tagsLine.empty()) t->setTags(splitTags(tagsLine));
        return "Задача обновлена.";
    }

    // --- пункт 4 ---
    string removeTask(int id) {
        if (list_.removeTask(id)) return "Задача #" + to_string(id) + " удалена.";
        return "Задача с номером " + to_string(id) + " не найдена.";
    }

    // --- пункты 5 и 6: сохранение и загрузка (Abstract Factory из ДЗ-5) ---
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
        int n = st->load(list_, s.getPathToFile());
        if (n < 0) return "Не удалось открыть файл: " + s.getPathToFile();
        return "Загружено задач: " + to_string(n) + ". Номера пересчитаны заново.";
    }

    // --- пункт 7: настройки (Singleton из ДЗ-5) ---
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

    // --- пункт 8: поиск (Strategy) ---
    vector<string> searchCriteria() const {
        vector<string> v;
        v.push_back("по тегу");
        v.push_back("по приоритету");
        v.push_back("по дате");
        v.push_back("по диапазону дат");
        v.push_back("по тексту в названии");
        return v;
    }

    // criterion — номер из searchCriteria(); value/value2 — введённые значения.
    // Фасад собирает нужную стратегию и передаёт её списку.
    vector<string> search(int criterion, const string& value, const string& value2) {
        vector<string> out;
        unique_ptr<SearchStrategy> s;
        if (criterion == 1) {
            if (value.empty()) { out.push_back("Тег не задан."); return out; }
            s.reset(new TagSearchStrategy(value));
        } else if (criterion == 2) {
            if (value == "1") s.reset(new PrioritySearchStrategy(Priority::High));
            else if (value == "2") s.reset(new PrioritySearchStrategy(Priority::Medium));
            else if (value == "3") s.reset(new PrioritySearchStrategy(Priority::Low));
            else { out.push_back("Приоритет не понял, введите 1, 2 или 3."); return out; }
        } else if (criterion == 3) {
            if (!isValidDate(value))
                { out.push_back("Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15."); return out; }
            s.reset(new DateSearchStrategy(value));
        } else if (criterion == 4) {
            if (value.empty() && value2.empty())
                { out.push_back("Задайте хотя бы одну границу диапазона."); return out; }
            if (!value.empty() && !isValidDate(value))
                { out.push_back("Нижняя дата должна быть ГГГГ-ММ-ДД."); return out; }
            if (!value2.empty() && !isValidDate(value2))
                { out.push_back("Верхняя дата должна быть ГГГГ-ММ-ДД."); return out; }
            s.reset(new DateRangeSearchStrategy(value, value2));
        } else {
            if (value.empty()) { out.push_back("Текст не задан."); return out; }
            s.reset(new TextSearchStrategy(value));
        }
        vector<Task*> found = list_.findBy(*s);
        if (found.empty()) { out.push_back("Ничего не найдено."); return out; }
        out.push_back("Найдено задач: " + to_string(found.size()));
        for (size_t i = 0; i < found.size(); ++i) out.push_back(taskText(found[i]));
        return out;
    }

    // --- пункт 9: создать из шаблона (Prototype) ---
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

    // --- пункт 10: импорт из внешнего списка (Adapter) ---
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
};

#endif // TASKFLOWFACADE_H
