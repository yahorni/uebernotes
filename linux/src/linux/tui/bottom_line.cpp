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
        if (!_inputBuffer.empty()) {
            std::string statusPrefix;
            switch (_mode) {
            case Mode::Search:
                statusPrefix = "Search: ";
                break;
            case Mode::Command:
                statusPrefix = "Command: ";
                break;
            case Mode::Status:
                Log::error("Input received in Status mode");
                _inputBuffer.clear();
                return;
            }

            _statusBuffer = statusPrefix + _inputBuffer;
            _eventQueue->push(Event::InputEntered, std::string(_statusBuffer), _inputBuffer);
            _inputBuffer.clear();
        }
        // TODO: add messages if input is empty
        _mode = Mode::Status;
    };

    inputOption.content = &_inputBuffer;
    inputOption.placeholder = &_inputPlaceholder;

    _inputLine = ftxui::Input(inputOption);
    _inputLine |= ftxui::Maybe([this] { return _mode != Mode::Status; });
    // TODO:
    // 1. handle Escape
    // 2. disable cursor during typing
}

void BottomLine::setMessage(std::string message) {
    _statusBuffer = std::move(message);
}

bool BottomLine::isInputActive() const { return _mode == Mode::Search || _mode == Mode::Command; }

void BottomLine::setMode(BottomLine::Mode mode) { _mode = mode; }

const ftxui::Component& BottomLine::getComponent() const { return _inputLine; }

ftxui::Element BottomLine::getElement() const {
    using namespace ftxui;

    Element line;
    switch (_mode) {
    case Mode::Search: {
        _inputPlaceholder = "search";
        line = hbox(text("/"), _inputLine->Render());
    } break;
    case Mode::Command: {
        _inputPlaceholder = "command";
        line = hbox(text(":"), _inputLine->Render());
    } break;
    case Mode::Status:
    default: {
        line = text(_statusBuffer.c_str());
    } break;
    }

    return line | size(HEIGHT, EQUAL, 1);
}
}  // namespace linux::tui
