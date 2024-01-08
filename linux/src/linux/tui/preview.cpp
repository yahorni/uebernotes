#include "linux/tui/preview.hpp"

#include "ftxui/component.hpp"
#include "linux/utils/noncopyable.hpp"

#include <string>
#include <utility>

namespace linux::tui::preview {

using ftxui::utils::OptionalRef;

class Model : private utils::NonCopyable {
public:
    // UI Component
    const ftxui::Component& getComponent() const { return _pager; }
    void setComponent(ftxui::Component pager) { _pager = pager; }

    // data
    void setNote(const core::NotePtr& notePtr) {
        _notePtr = notePtr;
        if (_notePtr) {
            _noteContent = std::make_optional(std::cref(_notePtr->getContent()));
        } else {
            _noteContent = std::nullopt;
        }
    }

    const OptionalCRef<std::string>& getContent() const { return _noteContent; }

private:
    core::NotePtr _notePtr;
    OptionalCRef<std::string> _noteContent;
    ftxui::Component _pager;
};

class View : private utils::NonCopyable {
public:
    ftxui::Component createComponent(const OptionalCRef<std::string>& noteContent) {
        return ftxui::Pager(noteContent, _previewShift, _wrapLines);
    }

    void resetShift() { _previewShift = 0; }

    ftxui::Element getElement(const ftxui::Component& component) const {
        using namespace ftxui;  // NOLINT
        return vbox({
                   hcenter(bold(text("Preview"))),        // consider using "window"
                   separator(),                           //
                   component->Render() | yflex | yframe,  //
               }) |
               xflex | borderDecorator(component->Focused());
    }

private:
    int _previewShift = 0;
    bool _wrapLines = true;
};

// Controller

Controller::Controller()
    : _model(std::make_unique<Model>()),
      _view(std::make_unique<View>()) {}

// needed here for unique_ptr destruction
Controller::~Controller() = default;

void Controller::createComponent(Communicator& communicator) {
    auto pager = _view->createComponent(_model->getContent());
    configureComponent(pager, communicator);
    _model->setComponent(std::move(pager));
}

void Controller::configureComponent(ftxui::Component& component, Communicator& communicator) {
    component |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Return) {
            communicator.cmdPush(Command::OpenEditor);
            communicator.ntfPush("TODO: Open editor");
            return true;
        }
        return false;
    });
}

void Controller::setNote(const core::NotePtr& notePtr) {
    _model->setNote(notePtr);
    _view->resetShift();
}

const ftxui::Component& Controller::component() const { return _model->getComponent(); }
ftxui::Element Controller::element() const { return _view->getElement(_model->getComponent()); }

}  // namespace linux::tui::preview
