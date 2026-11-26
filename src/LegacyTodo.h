// legacy_todo.h — «чужая» библиотека для варианта Adapter (менять нельзя)
#include <string>
#include <vector>

struct LegacyItem {
    std::string text;
    int prio;          // 0 — самый низкий, 9 — самый высокий
    long long dueUnix; // срок: секунды с 01.01.1970
};

class LegacyTodo {
public:
    std::vector<LegacyItem> exportAll() const {
        return { {"Купить билеты", 8, 1796083200}, {"Позвонить маме", 3, 1796169600} };
    }
};
