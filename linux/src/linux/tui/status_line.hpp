#pragma once

#include "linux/logger.hpp"
#include "linux/tui/event_queue.hpp"

#include <ftxui/component/component.hpp>

#include <string>
#include <utility>

namespace linux::tui {

class StatusLine {
public:
    explicit StatusLine(EventQueue* eventQueue)
        : _eventQueue(eventQueue) {}

    void setMessage(std::string message) {
        _lastMessage = std::move(message);
        Log::info("{}", _lastMessage);
    }

    // UI
    ftxui::Element getElement() const { return ftxui::text(_lastMessage.c_str()); }

private:
    EventQueue* _eventQueue{nullptr};
    std::string _lastMessage = "Welcome to uebernotes";
};

}  // namespace linux::tui
