#pragma once

#include "linux/tui/event_queue.hpp"

#include <ftxui/component/component.hpp>

#include <string>

namespace linux::tui {

class BottomLine {
public:
    enum class Mode {
        Status,
        Search,
        Command
    };

    explicit BottomLine(EventQueue* eventQueue);

    void setMessage(std::string message);
    bool isInputActive() const;
    void setMode(BottomLine::Mode mode);

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    EventQueue* _eventQueue{nullptr};

    std::string _statusBuffer = "Welcome to uebernotes";
    std::string _inputBuffer;
    mutable std::string _inputPlaceholder;
    Mode _mode = Mode::Status;

    ftxui::Component _inputLine;
};

}  // namespace linux::tui
