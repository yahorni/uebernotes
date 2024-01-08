#include "linux/tui/status.hpp"

#include "linux/logger.hpp"
#include "linux/utils/noncopyable.hpp"

#include <ftxui/component/component.hpp>

#include <utility>

namespace linux::tui::status {

// TODO: think about removing getters/setters, just make Controller friend class
class Model : private utils::NonCopyable {
public:
    Mode getMode() const { return _mode; }
    void setMode(Mode mode) { _mode = mode; }
    const std::string& getLastInput() const { return _lastInput; }
    void setLastInput(std::string input) { _lastInput = std::move(input); }

    // UI Component
    const ftxui::Component& getComponent() const { return _input; }
    void setComponent(ftxui::Component input) { _input = input; }

private:
    Mode _mode = Mode::Status;
    std::string _lastInput;

    ftxui::Component _input;
};

class View : private utils::NonCopyable {
public:
    const std::string& getStatusBuffer() const { return _statusBuffer; }
    void setStatusBuffer(std::string message) { _statusBuffer = std::move(message); }

    const std::string& getInputBuffer() const { return _inputBuffer; }
    void clearInputBuffer() { _inputBuffer.clear(); }

    ftxui::Component createComponent(ftxui::InputOption& option) {
        option.content = &_inputBuffer;
        option.placeholder = &_inputPlaceholder;
        option.multiline = false;
        return ftxui::Input(option);
    }

    ftxui::Element getElement(const ftxui::Component& component, Mode mode) const {
        using namespace ftxui;  // NOLINT

        Element line;
        switch (mode) {
        case Mode::Search: {
            _inputPlaceholder = "search";
            line = hbox(text("/"), component->Render());
        } break;
        case Mode::Command: {
            _inputPlaceholder = "command";
            line = hbox(text(":"), component->Render());
        } break;
        case Mode::Status:
        default: {
            line = text(_statusBuffer.c_str());
        } break;
        }

        return line | size(HEIGHT, EQUAL, 1);
    }

private:
    std::string _statusBuffer = "Welcome to uebernotes";
    std::string _inputBuffer;
    mutable std::string _inputPlaceholder;
};

// Controller

Controller::Controller()
    : _model(std::make_unique<Model>()),
      _view(std::make_unique<View>()) {}

// needed here for unique_ptr destruction
Controller::~Controller() = default;

void Controller::createComponent(Communicator& communicator) {
    auto option = ftxui::InputOption();
    configureComponentOption(option, communicator);
    auto input = _view->createComponent(option);
    configureComponent(input, communicator);
    _model->setComponent(std::move(input));
}

void Controller::configureComponentOption(ftxui::InputOption& option, Communicator& communicator) {
    option.on_enter = [&]() {
        if (_model->getMode() == Mode::Status) {
            Log::error("Input received in Status mode");
            _view->clearInputBuffer();
            communicator.cmdPush(Command::InputCanceled);
            return;
        }

        auto& input = _view->getInputBuffer();
        if (!input.empty()) {
            switch (_model->getMode()) {
            case Mode::Search:
                _view->setStatusBuffer(std::format("Search: {}", input));
                break;
            case Mode::Command:
                _view->setStatusBuffer(std::format("Command: {}", input));
                break;
            default:
                break;
            }

            _model->setLastInput(input);
            _view->clearInputBuffer();
            communicator.cmdPush(Command::InputEntered);

        } else {
            switch (_model->getMode()) {
            case Mode::Search:
                _view->setStatusBuffer("Empty search");
                break;
            case Mode::Command:
                _view->setStatusBuffer("Empty command");
                break;
            default:
                break;
            }
            communicator.cmdPush(Command::InputCanceled);
        }

        communicator.ntfPush(std::string(_view->getStatusBuffer()));
        _model->setMode(Mode::Status);
    };
}

void Controller::configureComponent(ftxui::Component& component, Communicator& communicator) {
    component |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Escape) {
            setMode(Mode::Status);
            _view->clearInputBuffer();
            communicator.cmdPush(Command::InputCanceled);
            communicator.ntfPush("Canceled input");
            return true;
        }
        return false;
    });
    component |= ftxui::Maybe([this] { return _model->getMode() != Mode::Status; });

    // TODO: disable cursor during typing
}

const std::string& Controller::getLastInput() const { return _model->getLastInput(); }
void Controller::setMessage(std::string message) { _view->setStatusBuffer(std::move(message)); }
void Controller::setMode(Mode mode) { _model->setMode(mode); }

const ftxui::Component& Controller::component() const { return _model->getComponent(); }
ftxui::Element Controller::element() const { return _view->getElement(_model->getComponent(), _model->getMode()); }

}  // namespace linux::tui::status
