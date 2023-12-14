#include "linux/tui/bottom_line.hpp"

#include "linux/logger.hpp"

#include <ftxui/component/component.hpp>

#include <utility>

namespace linux::tui {

BottomLine::BottomLine(EventQueue* eventQueue)
    : _eventQueue(eventQueue) {
    auto inputOption = ftxui::InputOption();
    inputOption.multiline = false;
    inputOption.on_enter = [&]() {
        std::string statusPrefix;
        if (_mode == Mode::Search) {
            statusPrefix = "Search: ";
        } else if (_mode == Mode::Command) {
            statusPrefix = "Command: ";
        } else {
            statusPrefix = "Unknown input: ";
        }

        _statusBuffer = statusPrefix + _inputBuffer;

        _eventQueue->push(Event::InputEntered, std::string(_statusBuffer), _inputBuffer);

        _inputBuffer.clear();
        _mode = Mode::Status;
    };

    _inputLine = ftxui::Input(&_inputBuffer, "input", inputOption);
    _inputLine |= ftxui::Maybe([this] { return _mode != Mode::Status; });
    // TODO:
    // 1. handle Escape
    // 2. disable cursor
}

void BottomLine::setMessage(std::string message) {
    _statusBuffer = std::move(message);
    Log::info("{}", _statusBuffer);
}

void BottomLine::setMode(BottomLine::Mode mode) { _mode = mode; }

const ftxui::Component& BottomLine::getComponent() const { return _inputLine; }

ftxui::Element BottomLine::getElement() const {
    using namespace ftxui;
    switch (_mode) {
    case Mode::Search: {
        return hbox(text("/"), _inputLine->Render());
    } break;
    case Mode::Command: {
        return hbox(text(":"), _inputLine->Render());
    } break;
    case Mode::Status:
    default: {
        return text(_statusBuffer.c_str());
    } break;
    }
}

}  // namespace linux::tui
