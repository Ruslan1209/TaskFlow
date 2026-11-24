#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include "Task.h"
#include "TaskList.h"
#include "TaskFilter.h"
#include "AppSettings.h"
#include "TaskFactory.h"
#include "TaskStorage.h"
using namespace std;

string readLine(const string& prompt) {
    cout << prompt;
    string s;
    getline(cin, s);
    return s;
}

// Единственный вход для чисел: буквы, мусор, пустота не ломают программу
int readInt(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        istringstream iss(s);
        int v; char extra;
        if (iss >> v && !(iss >> extra)) return v;
        cout << "Нужно целое число, попробуйте ещё раз.\n";
    }
}

// приоритет 1..3: чем меньше, тем важнее (из 01_vision.md)
Priority readPriority(const string& prompt) {
    while (true) {
        int v = readInt(prompt + " (1-высокий, 2-средний, 3-низкий): ");
        if (v >= 1 && v <= 3) {
            if (v == 1) return Priority::High;
            if (v == 2) return Priority::Medium;
            return Priority::Low;
        }
        cout << "Введите число от 1 до 3.\n";
    }
}

bool isValidDate(const string& d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (int i = 0; i < 10; ++i)
        if (i != 4 && i != 7 && (d[i] < '0' || d[i] > '9')) return false;
    int month = stoi(d.substr(5, 2));
    int day = stoi(d.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31;
}

// Enter — оставить прежнее значение (для редактирования)
string dateInput(const string& prompt, const string& current) {
    while (true) {
        string s = readLine(prompt);
        if (s.empty()) return current;
        if (isValidDate(s)) return s;
        cout << "Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15.\n";
    }
}

vector<string> splitTags(const string& line) {
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

void printTask(const Task* t) {
    cout << "#" << t->getId() << " [" << statusName(t->getStatus()) << "] "
         << t->getTitle()
         << " | тип: " << t->typeName()
         << " | приоритет: " << priorityName(t->getPriority())
         << " | срок: " << t->getDate();
    if (!t->getDescription().empty()) cout << " | " << t->getDescription();
    const vector<string>& tg = t->getTags();
    if (!tg.empty()) {
        cout << " | теги: ";
        for (size_t i = 0; i < tg.size(); ++i)
            cout << tg[i] << (i + 1 < tg.size() ? ", " : "");
    }
    if (!t->extraInfo().empty()) cout << " | " << t->extraInfo();
    cout << "\n";
}

int main() {
    TaskList list;
    while (true) {
        cout << "\n1-добавить 2-показать все 3-изменить 4-удалить "
                "5-сохранить 6-загрузить 7-настройки 8-поиск 0-выход\n";
        int cmd = readInt("Ваш выбор: ");
        if (cmd == 0) break;
        switch (cmd) {
        case 1: {
            // Factory Method: тип задачи выбирается через реестр создателей
            vector<string> types = TaskFactory::instance().typeNames();
            cout << "Тип задачи:\n";
            for (size_t i = 0; i < types.size(); ++i)
                cout << "  " << (i + 1) << "-" << types[i] << "\n";
            int tt = readInt("Тип: ");
            if (tt < 1 || tt > (int)types.size()) {
                cout << "Нет такого типа.\n";
                break;
            }
            unique_ptr<Task> t = TaskFactory::instance().create(types[tt - 1]);
            if (!t) { cout << "Не удалось создать задачу.\n"; break; }
            string title;
            while (true) {
                title = readLine("Название: ");
                if (!title.empty()) break;
                cout << "Название не может быть пустым.\n";
            }
            t->setTitle(title);
            t->setDescription(readLine("Описание: "));
            t->setPriority(readPriority("Приоритет"));
            if (t->typeName() == "повторяющаяся") {
                RecurringTask* r = dynamic_cast<RecurringTask*>(t.get());
                if (r) r->setInterval(readInt("Повторять каждые сколько дней: "));
                t->setDate(dateInput("Дата первого выполнения (ГГГГ-ММ-ДД): ", ""));
            } else if (t->typeName() == "со сроком") {
                t->setDate(dateInput("Дата дедлайна (ГГГГ-ММ-ДД): ", ""));
            } else {
                t->setDate(dateInput("Дата (ГГГГ-ММ-ДД): ", ""));
            }
            t->setTags(splitTags(readLine("Теги через запятую: ")));
            int id = list.addTask(move(t));
            cout << "Задача создана, номер " << id << ".\n";
            break;
        }
        case 2: {
            const vector<unique_ptr<Task>>& all = list.all();
            if (all.empty()) { cout << "Список пуст.\n"; break; }
            for (size_t i = 0; i < all.size(); ++i) printTask(all[i].get());
            break;
        }
        case 3: {
            int id = readInt("Номер задачи для изменения: ");
            Task* t = list.find(id);
            if (!t) { cout << "Задача с номером " << id << " не найдена.\n"; break; }
            cout << "Редактируем задачу #" << id << " (Enter — оставить старое).\n";
            string s = readLine("Название [" + t->getTitle() + "]: ");
            if (!s.empty()) t->setTitle(s);
            s = readLine("Описание [" + t->getDescription() + "]: ");
            if (!s.empty()) t->setDescription(s);
            s = readLine("Приоритет (1-высокий, 2-средний, 3-низкий) [" +
                         priorityName(t->getPriority()) + "]: ");
            if (!s.empty()) {
                if (s == "1") t->setPriority(Priority::High);
                else if (s == "2") t->setPriority(Priority::Medium);
                else if (s == "3") t->setPriority(Priority::Low);
                else cout << "Приоритет не понял, оставляю старый.\n";
            }
            if (t->typeName() != "обычная")
                t->setDate(dateInput("Дата [" + t->getDate() + "]: ", t->getDate()));
            s = readLine("Теги через запятую (Enter — оставить): ");
            if (!s.empty()) t->setTags(splitTags(s));
            cout << "Задача обновлена.\n";
            break;
        }
        case 4: {
            int id = readInt("Номер задачи для удаления: ");
            if (list.removeTask(id)) cout << "Задача #" << id << " удалена.\n";
            else cout << "Задача с номером " << id << " не найдена.\n";
            break;
        }
        case 5: {
            // Abstract Factory: хранилище задач из семейства настроек
            AppSettings& s = AppSettings::instance();
            unique_ptr<ITaskStorage> st = s.factory().createTaskStorage();
            if (st->save(list, s.getPathToFile()))
                cout << "Сохранено в " << s.getPathToFile()
                     << " (формат " << s.getFormat() << ").\n";
            else
                cout << "Не удалось открыть файл для записи: "
                     << s.getPathToFile() << "\n";
            break;
        }
        case 6: {
            AppSettings& s = AppSettings::instance();
            unique_ptr<ITaskStorage> st = s.factory().createTaskStorage();
            if (!list.all().empty()) {
                string a = readLine("Список не пуст. Загрузить вместо текущего? (y/n): ");
                if (a != "y" && a != "Y" && a != "д" && a != "Д") break;
            }
            list.clear();
            int n = st->load(list, s.getPathToFile());
            if (n < 0)
                cout << "Не удалось открыть файл: " << s.getPathToFile() << "\n";
            else
                cout << "Загружено задач: " << n
                     << ". Номера пересчитаны заново.\n";
            break;
        }
        case 7: {
            // Singleton: всё через единственный экземпляр AppSettings
            AppSettings& s = AppSettings::instance();
            cout << "\nНастройки. Текущий формат: " << s.getFormat()
                 << " | путь к файлу: " << s.getPathToFile() << "\n";
            int what = readInt("1-сменить формат 2-сменить путь 0-назад: ");
            if (what == 1) {
                string f = readLine("Новый формат (txt или csv): ");
                if (f == "txt" || f == "csv") {
                    s.setFormat(f);
                    cout << "Формат изменён на " << f << ".\n";
                } else {
                    cout << "Неверный формат, допустимы только txt и csv. "
                            "Настройка не изменена.\n";
                }
            } else if (what == 2) {
                string p = readLine("Новый путь к файлу: ");
                if (!p.empty()) {
                    s.setPathToFile(p);
                    cout << "Путь изменён.\n";
                } else {
                    cout << "Путь не может быть пустым.\n";
                }
            }
            break;
        }
        case 8: 
            {
            // Поиск через TaskFilter: Enter — пропустить критерий
            TaskFilter f;
            bool any = false;
            string s = readLine("Тег для поиска (Enter — пропустить): ");
            if (!s.empty()) { f.setTag(s); any = true; }
            s = readLine("Дата для поиска ГГГГ-ММ-ДД (Enter — пропустить): ");
            if (!s.empty()) {
                if (!isValidDate(s)) {
                    cout << "Дата неверная, поиск по дате пропущен.\n";
                } else {
                    f.setDate(s);
                    any = true;
                }
            }
            s = readLine("Приоритет 1-высокий, 2-средний, 3-низкий (Enter — пропустить): ");
            if (!s.empty()) {
                if (s == "1") { f.setPriority(Priority::High); any = true; }
                else if (s == "2") { f.setPriority(Priority::Medium); any = true; }
                else if (s == "3") { f.setPriority(Priority::Low); any = true; }
                else cout << "Приоритет не понял, поиск по приоритету пропущен.\n";
            }
            if (!any)
                cout << "Задайте хотя бы один критерий.\n";
            else {
                vector<Task*> found = list.findBy(f);
                if (found.empty())
                    cout << "Ничего не найдено.\n";
                else {
                    cout << "Найдено задач: " << found.size() << "\n";
                    for (size_t i = 0; i < found.size(); ++i) printTask(found[i]);
                }
            }
            break;
        } 
        }    
        default: cout << "Нет такого пункта.\n";
        }
    }
    cout << "Пока!\n";
}
