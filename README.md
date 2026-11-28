

Консольное приложение «список задач»: создание, редактирование, поиск,
сортировка, сохранение в файл, отмена и повтор действий.

## Сборка и запуск

Проект собирается в Visual Studio 2022 (C++, консольное приложение).
Открыть .sln → Ctrl+Shift+B → Ctrl+F5.

## Возможности (пункты меню)

1 — создать задачу (обычная, повторяющаяся — через Builder и фабрику)
2 — показать все (с сортировкой: по сроку, приоритету, названию)
3 — редактировать задачу
4 — удалить задачу
5 — сохранить в файл
6 — загрузить из файла
7 — настройки (формат txt/csv, путь к файлу)
8 — поиск (по тегу, приоритету, дате, диапазону дат, тексту)
9 — шаблоны задач
10 — импорт из старого формата
11 — отменить последнее действие
12 — повторить отменённое
13 — загрузить из файла старого формата
14 — включить/выключить автосохранение

## Паттерны в проекте

| Паттерн | Где | Файлы |
|---|---|---|
| Builder | пошаговое создание задачи с валидацией | TaskBuilder.h |
| Factory Method | создание задач по имени типа | TaskFactory.h |
| Abstract Factory | семейства хранилищ txt/csv | TaskStorage.h |
| Prototype | шаблоны задач через clone() | TaskTemplates.h |
| Singleton | настройки и конфигурация | AppSettings.h, AppConfig.h |
| Facade | единая точка входа для меню | TaskFlowFacade.h |
| Adapter | импорт старых форматов | LegacyTodoAdapter.h, LegacyFileAdapter.h |
| Strategy | поиск и сортировка | TaskSearchStrategy.h, TaskSortStrategy.h |
| Observer | автосохранение, проверка сроков | TaskObserver.h, TaskObservers.h |
| Command | отмена и повтор действий | TaskCommand.h |

Подробности — в docs/PATTERNS.md и docs/07_why.md.

## Структура

TaskFlow/
  src/        — исходный код
  docs/       — PATTERNS.md, 07_why.md
  docs/uml/   — диаграммы PlantUML (.puml + .png)

## Документация

- docs/PATTERNS.md — карта паттернов проекта (ДЗ-1…ДЗ-7)
- docs/07_why.md — обоснование выбора паттернов ДЗ-07
- docs/uml/07_behavioral.puml — классы Strategy, Observer, Command (+ .png)
- docs/uml/07_seq_undo.puml — последовательность «удалить → отменить» (+ .png)
- docs/uml/02_class_domain.puml — доменная модель всего проекта
ME (2).md…]()

