#pragma once
#include <functional>
#include <string>
#include <vector>

struct GameEvent {
    enum class Type { Notification, Warning, Crisis, Opportunity };
    Type        type;
    std::string title;
    std::string body;
    int         cycle = 0;
};

using EventCallback = std::function<void(const GameEvent&)>;

class EventSystem {
public:
    void subscribe(EventCallback cb);
    void emit(GameEvent event);
    const std::vector<GameEvent>& history() const { return m_history; }

private:
    std::vector<EventCallback> m_callbacks;
    std::vector<GameEvent>     m_history;
};
