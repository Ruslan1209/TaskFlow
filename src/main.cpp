// Egednewnik

#include <iostream>
#include <string>
#include "TaskFlowFacade.h"
#include <cstdlib>
using namespace std;

int main() 
{
    system("chcp 1251 > NUL");

    TaskFlowFacade facade;
    string line;
    string title, desc, prio, status, date, tagsLine;
    cout << "=== Менеджер задач ===" << endl;
    while (true) {
        cout << "\nМеню:" << endl;
        cout << "1 — добавить задачу" << endl;
        cout << "2 — показать все задачи" << endl;
        cout << "3 — изменить задачу" << endl;
        cout << "4 — удалить задачу" << endl;
        cout << "5 — сохранить в файл" << endl;
        cout << "6 — загрузить из файла" << endl;
        cout << "7 — настройки" << endl;
        cout << "8 — поиск" << endl;
        cout << "9 — создать из шаблона" << endl;
        cout << "10 — импорт из внешнего списка" << endl;
        cout << "11 — отменить последнее действие" << endl;
        cout << "12 — повторить отменённое" << endl;
        cout << "0 — выход" << endl;
        cout << "Ваш выбор: ";

        string line;
        if (!getline(cin, line)) break;
        int choice = atoi(line.c_str());

        try {
            if (choice == 1) {
                vector<string> types = facade.taskTypes();
                cout << "Тип задачи:" << endl;
                for (size_t i = 0; i < types.size(); ++i)
                    cout << "  " << (i + 1) << " — " << types[i] << endl;
                cout << "Тип: ";
                getline(cin, line);
                int ti = atoi(line.c_str());
                if (ti < 1 || ti >(int)types.size()) {
                    cout << "Нет такого типа." << endl;
                    continue;
                }
                string type = types[ti - 1];
                cout << "Название: ";
                getline(cin, title);
                cout << "Описание: ";
                getline(cin, desc);
                cout << "Приоритет (1 — высокий, 2 — средний, 3 — низкий): ";
                getline(cin, line);
                int p = atoi(line.c_str());
                Priority prio = (p == 1 ? Priority::High :
                    p == 3 ? Priority::Low : Priority::Medium);
                cout << "Дата (ГГГГ-ММ-ДД): ";
                getline(cin, date);
                cout << "Теги через запятую: ";
                getline(cin, tagsLine);
                int interval = 0;
                if (type == "повторяющаяся") {
                    cout << "Интервал в днях: ";
                    getline(cin, line);
                    interval = atoi(line.c_str());
                }
                int id = facade.addTask(type, title, desc, prio, date, tagsLine, interval);
                cout << "Задача создана, номер " << id << "." << endl;
            }
            else if (choice == 2) {
                if (!facade.hasTasks()) {
                    cout << "Задач пока нет." << endl;
                    continue;
                }
                vector<string> sorts = facade.sortNames();
                cout << "Сортировка:" << endl;
                for (size_t i = 0; i < sorts.size(); ++i)
                    cout << "  " << (i + 1) << " — " << sorts[i] << endl;
                cout << "Сортировка: ";
                getline(cin, line);
                vector<string> rows = facade.allText(atoi(line.c_str()) - 1);
                for (size_t i = 0; i < rows.size(); ++i)
                    cout << rows[i] << endl;
            }
            else if (choice == 3) {
                cout << "Номер задачи: ";
                getline(cin, line);
                int id = atoi(line.c_str());
                if (!facade.hasTask(id)) {
                    cout << "Задача не найдена." << endl;
                    continue;
                }
                cout << "Оставь поле пустым, чтобы не менять. Задача #"
                    << id << " (" << facade.typeNameOf(id) << ")" << endl;
                cout << "Название [" << facade.fieldOf(id, "title") << "]: ";
                getline(cin, title);
                cout << "Описание [" << facade.fieldOf(id, "description") << "]: ";
                getline(cin, desc);
                cout << "Приоритет (1/2/3) [" << facade.fieldOf(id, "priority") << "]: ";
                getline(cin, prio);
                cout << "Статус (1 — новая, 2 — в работе, 3 — готова) ["
                    << facade.fieldOf(id, "status") << "]: ";
                getline(cin, status);
                cout << "Дата [" << facade.fieldOf(id, "date") << "]: ";
                getline(cin, date);
                cout << "Теги через запятую: ";
                getline(cin, tagsLine);
                cout << facade.editTask(id, title, desc, prio, status, date, tagsLine) << endl;
            }
            else if (choice == 4) {
                cout << "Номер задачи: ";
                getline(cin, line);
                cout << facade.removeTask(atoi(line.c_str())) << endl;
            }
            else if (choice == 5) {
                cout << facade.saveAll() << endl;
            }
            else if (choice == 6) {
                if (facade.hasTasks()) {
                    cout << "Список не пуст — загрузить и заменить его? (y/n): ";
                    getline(cin, line);
                    if (line != "y" && line != "Y") continue;
                }
                cout << facade.loadAll() << endl;
            }
            else if (choice == 7) {
                cout << facade.settingsText() << endl;
                cout << "1 — сменить формат, 2 — сменить путь, другое — назад: ";
                getline(cin, line);
                if (line == "1") {
                    cout << "Формат (txt или csv): ";
                    getline(cin, line);
                    cout << facade.setFormat(line) << endl;
                }
                else if (line == "2") {
                    cout << "Новый путь к файлу: ";
                    getline(cin, line);
                    cout << facade.setPath(line) << endl;
                }
            }
            else if (choice == 8) {
                vector<string> cr = facade.searchCriteria();
                cout << "Критерий:" << endl;
                for (size_t i = 0; i < cr.size(); ++i)
                    cout << "  " << (i + 1) << " — " << cr[i] << endl;
                cout << "Критерий: ";
                getline(cin, line);
                int c = atoi(line.c_str());
                string v1, v2;
                if (c == 4) {
                    cout << "Дата от (пусто — без нижней границы): ";
                    getline(cin, v1);
                    cout << "Дата до (пусто — без верхней границы): ";
                    getline(cin, v2);
                }
                else if (c > 0) {
                    cout << "Значение: ";
                    getline(cin, v1);
                }
                else continue;
                vector<string> rows = facade.search(c, v1, v2);
                for (size_t i = 0; i < rows.size(); ++i)
                    cout << rows[i] << endl;
            }
            else if (choice == 9) {
                vector<string> tmpls = facade.templateTexts();
                cout << "Шаблоны:" << endl;
                for (size_t i = 0; i < tmpls.size(); ++i)
                    cout << "  " << (i + 1) << " — " << tmpls[i] << endl;
                cout << "Шаблон: ";
                getline(cin, line);
                int ti = atoi(line.c_str());
                cout << "Название новой задачи: ";
                getline(cin, title);
                int id = facade.addFromTemplate(ti - 1, title);
                cout << "Задача создана, номер " << id << "." << endl;
            }
            else if (choice == 10) {
                vector<string> rows = facade.importLegacy();
                if (rows.empty()) cout << "Нечего импортировать." << endl;
                for (size_t i = 0; i < rows.size(); ++i)
                    cout << rows[i] << endl;
            }
            else if (choice == 11) {
                cout << facade.undo() << endl;
            }
            else if (choice == 12) {
                cout << facade.redo() << endl;
            }
            else if (choice == 0) {
                cout << "До встречи!" << endl;
                break;
            }
            else {
                cout << "Нет такого пункта." << endl;
            }
        }
        catch (const exception& e) {
            cout << "Ошибка: " << e.what() << endl;
        }
    }
}

