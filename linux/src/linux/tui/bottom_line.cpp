#include "linux/tui/bottom_line.hpp"

#include "linux/logger.hpp"

#include <ftxui/component/component.hpp>

#include <utility>

namespace linux::tui {

BottomLine::BottomLine(Communicator& communicator) {
    auto inputOption = ftxui::InputOption();
    inputOption.multiline = false;
    inputOption.on_enter = [&]() {
        if (_mode == Mode::Status) {
            Log::error("Input received in Status mode");
            _inputBuffer.clear();
            communicator.cmdPush(Command::InputCanceled);
            return;
        }

        if (!_inputBuffer.empty()) {
            switch (_mode) {
            case Mode::Search:
                _statusBuffer = std::format("Search: {}", _inputBuffer);
                break;
            case Mode::Command:
                _statusBuffer = std::format("Command: {}", _inputBuffer);
                break;
            default:
                break;
            }

            _lastInput = std::move(_inputBuffer);
            _inputBuffer.clear();

            communicator.cmdPush(Command::InputEntered);
            communicator.ntfPush(std::string(_statusBuffer));
        } else {
            switch (_mode) {
            case Mode::Search:
                _statusBuffer = "Empty search";
                break;
            case Mode::Command:
                _statusBuffer = "Empty command";
                break;
            default:
                break;
            }
            communicator.cmdPush(Command::InputCanceled);
            communicator.ntfPush(std::string(_statusBuffer));
        }

        _mode = Mode::Status;
    };

    inputOption.content = &_inputBuffer;
    inputOption.placeholder = &_inputPlaceholder;

    _inputLine = ftxui::Input(inputOption);
    _inputLine |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Escape) {
            setMode(Mode::Status);
            _inputBuffer.clear();
            communicator.cmdPush(Command::InputCanceled);
            communicator.ntfPush("Canceled input");
            return true;
        }
        return false;
    });
    _inputLine |= ftxui::Maybe([this] { return _mode != Mode::Status; });

    // TODO: disable cursor during typing
}

void BottomLine::setMessage(std::string message) { _statusBuffer = std::move(message); }

bool BottomLine::isInputActive() const { return _mode == Mode::Search || _mode == Mode::Command; }

const std::string& BottomLine::getLastInput() const { return _lastInput; }

void BottomLine::setMode(BottomLine::Mode mode) { _mode = mode; }

const ftxui::Component& BottomLine::getComponent() const { return _inputLine; }

ftxui::Element BottomLine::getElement() const {
    using namespace ftxui;  // NOLINT

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
