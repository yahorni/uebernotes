#include "linux/tui/history.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <string>
#include <utility>
#include <vector>

namespace linux::tui::history {

class Model : private utils::NonCopyable {
public:
    Model() { _messages.reserve(_maxHistorySize); }

    // UI Component
    const ftxui::Component& getComponent() const { return _pager; }
    void setComponent(ftxui::Component pager) { _pager = pager; }

    // data
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

    ftxui::Component _pager;
};

class View : private utils::NonCopyable {
public:
    ftxui::Component createComponent(const std::vector<std::string>& messages) {
        return ftxui::Pager(messages, _previewShift, _wrapLines);
    }

    bool toggle() {
        _enabled = !_enabled;
        return _enabled;
    }

    bool isEnabled() const { return _enabled; }

    ftxui::Element getElement(const ftxui::Component& component, int height) const {
        if (_enabled) {
            return component->Render() | ftxui::borderDecorator(component->Focused()) |
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
    : _model(std::make_unique<Model>()),
      _view(std::make_unique<View>()) {}

// needed here for unique_ptr destruction
Controller::~Controller() = default;

void Controller::createComponent(Communicator& communicator) {
    auto pager = _view->createComponent(_model->getMessages());
    configureComponent(pager, communicator);
    _model->setComponent(std::move(pager));
}

void Controller::configureComponent(ftxui::Component& component, Communicator&) {
    component |= ftxui::Maybe([this] { return _view->isEnabled(); });
}

bool Controller::toggleView() { return _view->toggle(); }

void Controller::addMessage(std::string message) { _model->addMessage(std::move(message)); }

const ftxui::Component& Controller::component() const { return _model->getComponent(); }
ftxui::Element Controller::element(int height) const { return _view->getElement(_model->getComponent(), height); }

}  // namespace linux::tui::history
