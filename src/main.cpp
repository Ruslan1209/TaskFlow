#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "TaskFlowFacade.h"
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

// Enter — оставить прежнее значение (для редактирования)
string dateInput(const string& prompt, const string& current) {
    while (true) {
        string s = readLine(prompt);
        if (s.empty()) return current;
        if (isValidDate(s)) return s;
        cout << "Дата должна быть ГГГГ-ММ-ДД, например 2026-12-15.\n";
    }
}

int main() {
    // Facade: меню знает только фасад, все подсистемы спрятаны внутри
    TaskFlowFacade app;
    while (true) {
        cout << "\n1-добавить 2-показать все 3-изменить 4-удалить "
                "5-сохранить 6-загрузить 7-настройки 8-поиск "
                "9-создать из шаблона 10-импорт из внешнего списка 0-выход\n";
        int cmd = readInt("Ваш выбор: ");
        if (cmd == 0) break;
        switch (cmd) {
        case 1: {
            // Factory Method: тип выбирается через реестр создателей
            vector<string> types = app.taskTypes();
            cout << "Тип задачи:\n";
            for (size_t i = 0; i < types.size(); ++i)
                cout << "  " << (i + 1) << "-" << types[i] << "\n";
            int tt = readInt("Тип: ");
            if (tt < 1 || tt > (int)types.size()) {
                cout << "Нет такого типа.\n";
                break;
            }
            string title;
            while (true) {
                title = readLine("Название: ");
                if (!title.empty()) break;
                cout << "Название не может быть пустым.\n";
            }
            string desc = readLine("Описание: ");
            Priority prio = readPriority("Приоритет");
            int interval = -1;
            if (types[tt - 1] == "повторяющаяся")
                interval = readInt("Повторять каждые сколько дней: ");
            string date;
            if (types[tt - 1] == "повторяющаяся")
                date = dateInput("Дата первого выполнения (ГГГГ-ММ-ДД): ", "");
            else if (types[tt - 1] == "со сроком")
                date = dateInput("Дата дедлайна (ГГГГ-ММ-ДД): ", "");
            else
                date = dateInput("Дата (ГГГГ-ММ-ДД): ", "");
            string tagsLine = readLine("Теги через запятую: ");
            try {
                int id = app.addTask(types[tt - 1], title, desc, prio, date, tagsLine, interval);
                cout << "Задача создана, номер " << id << ".\n";
            } catch (const exception& e) {
                cout << e.what() << "\n";
            }
            break;
        }
        case 2: {
            // Strategy: показ всех с выбором сортировки
            vector<string> sorts = app.sortNames();
            cout << "Сортировка:\n";
            for (size_t i = 0; i < sorts.size(); ++i)
                cout << "  " << (i + 1) << "-" << sorts[i] << "\n";
            int sm = readInt("Сортировка: ");
            if (sm < 1 || sm > (int)sorts.size()) {
                cout << "Нет такой сортировки.\n";
                break;
            }
            vector<string> all = app.allText(sm - 1);
            if (all.empty()) { cout << "Список пуст.\n"; break; }
            for (size_t i = 0; i < all.size(); ++i) cout << all[i] << "\n";
            break;
        }
        case 3: {
            int id = readInt("Номер задачи для изменения: ");
            if (!app.hasTask(id)) {
                cout << "Задача с номером " << id << " не найдена.\n";
                break;
            }
            cout << "Редактируем задачу #" << id << " (Enter — оставить старое).\n";
            string title = readLine("Название [" + app.fieldOf(id, "title") + "]: ");
            string desc = readLine("Описание [" + app.fieldOf(id, "description") + "]: ");
            string prio = readLine("Приоритет (1-высокий, 2-средний, 3-низкий) ["
                                   + app.fieldOf(id, "priority") + "]: ");
            string status = readLine("Статус 1-новая, 2-в работе, 3-выполнена ["
                                     + app.fieldOf(id, "status") + "] (Enter — оставить): ");
            string date;
            if (app.typeNameOf(id) != "обычная")
                date = dateInput("Дата [" + app.fieldOf(id, "date") + "]: ",
                                 app.fieldOf(id, "date"));
            string tagsLine = readLine("Теги через запятую (Enter — оставить): ");
            cout << app.editTask(id, title, desc, prio, status, date, tagsLine) << "\n";
            break;
        }
        case 4: {
            int id = readInt("Номер задачи для удаления: ");
            cout << app.removeTask(id) << "\n";
            break;
        }
        case 5:
            cout << app.saveAll() << "\n";
            break;
        case 6: {
            if (app.hasTasks()) {
                string a = readLine("Список не пуст. Загрузить вместо текущего? (y/n): ");
                if (a != "y" && a != "Y" && a != "д" && a != "Д") break;
            }
            cout << app.loadAll() << "\n";
            break;
        }
        case 7: {
            cout << "\n" << app.settingsText() << "\n";
            int what = readInt("1-сменить формат 2-сменить путь 0-назад: ");
            if (what == 1) {
                string f = readLine("Новый формат (txt или csv): ");
                cout << app.setFormat(f) << "\n";
            } else if (what == 2) {
                string p = readLine("Новый путь к файлу: ");
                cout << app.setPath(p) << "\n";
            }
            break;
        }
        case 8: {
            // Strategy: выбор критерия -> одна стратегия -> список ищет сам
            vector<string> crits = app.searchCriteria();
            cout << "Критерий поиска:\n";
            for (size_t i = 0; i < crits.size(); ++i)
                cout << "  " << (i + 1) << "-" << crits[i] << "\n";
            int c = readInt("Критерий: ");
            if (c < 1 || c > (int)crits.size()) {
                cout << "Нет такого критерия.\n";
                break;
            }
            string value, value2;
            if (c == 1) value = readLine("Тег: ");
            else if (c == 2) value = readLine("Приоритет 1-высокий, 2-средний, 3-низкий: ");
            else if (c == 3) value = readLine("Дата ГГГГ-ММ-ДД: ");
            else if (c == 4) {
                value = readLine("Дата от ГГГГ-ММ-ДД (Enter — без нижней границы): ");
                value2 = readLine("Дата до ГГГГ-ММ-ДД (Enter — без верхней границы): ");
            } else value = readLine("Текст в названии: ");
            vector<string> out = app.search(c, value, value2);
            for (size_t i = 0; i < out.size(); ++i) cout << out[i] << "\n";
            break;
        }
        case 9: {
            // Prototype: выбор шаблона, у копии своё название
            vector<string> texts = app.templateTexts();
            cout << "Шаблоны:\n";
            for (size_t i = 0; i < texts.size(); ++i)
                cout << "  " << (i + 1) << ". " << texts[i] << "\n";
            int ti = readInt("Шаблон: ");
            if (ti < 1 || ti > (int)texts.size()) {
                cout << "Нет такого шаблона.\n";
                break;
            }
            string title;
            while (true) {
                title = readLine("Название задачи: ");
                if (!title.empty()) break;
                cout << "Название не может быть пустым.\n";
            }
            try {
                int id = app.addFromTemplate(ti - 1, title);
                cout << "Задача создана из шаблона, номер " << id << ".\n";
                cout << "Проверка копии: измените теги задачи #" << id
                     << " в пункте 3 — шаблон останется прежним.\n";
            } catch (const exception& e) {
                cout << e.what() << "\n";
            }
            break;
        }
        case 10: {
            // Adapter: импорт из «чужой» библиотеки LegacyTodo
            vector<string> imported = app.importLegacy();
            if (imported.empty()) {
                cout << "Внешний список пуст.\n";
            } else {
                cout << "Импортировано из внешнего списка: " << imported.size() << "\n";
                for (size_t i = 0; i < imported.size(); ++i)
                    cout << imported[i] << "\n";
            }
            break;
        }
        default: cout << "Нет такого пункта.\n";
        }
    }
    cout << "Пока!\n";
}
