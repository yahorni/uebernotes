#pragma once

#include "linux/tui/communicator.hpp"

#include <ftxui/component/component.hpp>

#include <string>

namespace linux::tui {

class BottomLine {
public:
    enum class Mode { Status, Search, Command };

    explicit BottomLine(Communicator& eventQueue);

    void setMessage(std::string message);
    bool isInputActive() const;
    const std::string& getLastInput() const;
    void setMode(BottomLine::Mode mode);

    // UI
    const ftxui::Component& getComponent() const;
    ftxui::Element getElement() const;

private:
    std::string _statusBuffer = "Welcome to uebernotes";
    std::string _inputBuffer;
    std::string _lastInput;
    mutable std::string _inputPlaceholder;
    Mode _mode = Mode::Status;

    ftxui::Component _inputLine;
};

}  // namespace linux::tui
