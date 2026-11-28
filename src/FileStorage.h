#pragma once
// FileStorage.h
// Хранилище из предыдущих домашних заданий. Оставлено в проекте,
// методы приведены к общему интерфейсу ITaskStorage.
#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include <string>
#include "TaskStorage.h"
using namespace std;

class FileStorage : public ITaskStorage {
public:
    bool save(const TaskList& list, const string& path) const {
        TxtTaskStorage txt;
        return txt.save(list, path);
    }
    int load(TaskList& list, const string& path) const {
        TxtTaskStorage txt;
        return txt.load(list, path);
    }
};
#endif // FILESTORAGE_H
