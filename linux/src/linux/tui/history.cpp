#include "linux/tui/history.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <string>
#include <utility>
#include <vector>

namespace linux::tui::history {

class Model : public mvc::Model {
public:
    Model() { _messages.reserve(_maxHistorySize); }

    const std::vector<std::string>& getMessages() const { return _messages; }
    void addMessage(std::string&& message) {
        if (_messages.size() == _maxHistorySize) {
            _messages.erase(_messages.begin());
            Log::debug("Pop message from history");
        }

        _messages.push_back(std::move(message));
        Log::debug("Push message to history");
    }

private:
    const std::size_t _maxHistorySize = 50;
    std::vector<std::string> _messages;
};

class View : public mvc::View {
public:
    ftxui::Component& createComponent(const std::vector<std::string>& messages) {
        _component = ftxui::Pager(messages, _previewShift, _wrapLines);
        return _component;
    }

    bool toggle() {
        _enabled = !_enabled;
        return _enabled;
    }

    bool isEnabled() const { return _enabled; }

    // UI element
    ftxui::Element element(int height) const {
        if (_enabled) {
            return _component->Render() | ftxui::borderDecorator(_component->Focused()) |
                   ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, height);
        } else {
            return ftxui::emptyElement();
        }
    }

private:
    bool _enabled = false;

    int _previewShift = 0;
    bool _wrapLines = false;
};

// Controller

Controller::Controller()
    : mvc::Controller("History"),
      _model(std::make_unique<Model>()),
      _view(std::make_unique<View>()) {}

// needed here for unique_ptr destruction
Controller::~Controller() = default;

void Controller::createComponent(Communicator& communicator) {
    auto& pager = _view->createComponent(_model->getMessages());
    configureComponent(pager, communicator);
}

void Controller::configureComponent(ftxui::Component& component, Communicator&) {
    component |= ftxui::Maybe([this] { return _view->isEnabled(); });
}

bool Controller::toggleView() { return _view->toggle(); }

void Controller::addMessage(std::string message) { _model->addMessage(std::move(message)); }

const ftxui::Component& Controller::component() const { return _view->component(); }
ftxui::Element Controller::element(int height) const { return _view->element(height); }

}  // namespace linux::tui::history
