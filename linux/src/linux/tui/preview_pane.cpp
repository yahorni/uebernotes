#include "linux/tui/preview_pane.hpp"

#include "ftxui/component.hpp"

#include <utility>

namespace linux::tui::preview {

// Controller

Controller::Controller() = default;

void Controller::createComponent(Communicator& communicator) {
    auto pager = _view.createComponent();
    configureComponent(pager, communicator);
    _model.setComponent(std::move(pager));
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
    _model.setNote(notePtr);
    _view.resetShift();
    if (notePtr) {
        _view.setContent(notePtr->getContent());
    } else {
        _view.clearContent();
    }
}

const ftxui::Component& Controller::component() const { return _model.getComponent(); }
ftxui::Element Controller::element() const { return _view.getElement(_model.getComponent()); }

namespace detail {

// Model
const ftxui::Component& Model::getComponent() const { return _pager; }
void Model::setComponent(ftxui::Component pager) { _pager = pager; }
void Model::setNote(const core::NotePtr& notePtr) { _notePtr = notePtr; }

// View
ftxui::Component View::createComponent() { return ftxui::Pager(_noteContent, _previewShift, _wrapLines); }

void View::resetShift() { _previewShift = 0; }
void View::clearContent() { _noteContent.clear(); }
void View::setContent(const std::string& content) { _noteContent = content; }

ftxui::Element View::getElement(const ftxui::Component& component) const {
    using namespace ftxui;  // NOLINT
    return vbox({
               hcenter(bold(text("Preview"))),        // consider using "window"
               separator(),                           //
               component->Render() | yflex | yframe,  //
           }) |
           xflex | borderDecorator(component->Focused());
}

}  // namespace detail

}  // namespace linux::tui::preview
