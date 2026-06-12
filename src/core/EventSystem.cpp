#include "EventSystem.hpp"

void EventSystem::subscribe(EventCallback cb) {
    m_callbacks.push_back(std::move(cb));
}

void EventSystem::emit(GameEvent event) {
    m_history.push_back(event);
    for (auto& cb : m_callbacks)
        cb(m_history.back());
}
