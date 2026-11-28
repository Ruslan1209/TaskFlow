#pragma once
// AppConfig.h
// Паттерн Singleton (шаг 5): переключатель автосохранения.
// Конструктор закрыт, доступ только через instance(), копировать нельзя.
// Старый AppSettings из предыдущих ДЗ не трогаем — он продолжает работать.
#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <string>
using namespace std;

class AppConfig {
public:
    static AppConfig& instance() {
        static AppConfig s;
        return s;
    }

    bool getAutoSave() const { return autoSave_; }
    void setAutoSave(bool v) { autoSave_ = v; }

private:
    AppConfig() : autoSave_(true) {}
    AppConfig(const AppConfig&);            // копировать нельзя
    AppConfig& operator=(const AppConfig&); // присваивать нельзя

    bool autoSave_;
};
#endif // APPCONFIG
