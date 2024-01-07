#include "linux/tui/preview.hpp"

#include "ftxui/component.hpp"

#include <string>
#include <utility>

namespace linux::tui::preview {

class Model {
public:
    Model() = default;
    Model(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) = delete;

    // UI Component
    const ftxui::Component& getComponent() const { return _pager; }
    void setComponent(ftxui::Component pager) { _pager = pager; }

    // data
    void setNote(const core::NotePtr& notePtr) { _notePtr = notePtr; }

private:
    core::NotePtr _notePtr;
    ftxui::Component _pager;
};

class View {
public:
    View() = default;
    View(const View&) = delete;
    View(View&&) = delete;
    View& operator=(const View&) = delete;
    View& operator=(View&&) = delete;

    ftxui::Component createComponent() { return ftxui::Pager(_noteContent, _previewShift, _wrapLines); }

    void resetShift() { _previewShift = 0; }
    void clearContent() { _noteContent.clear(); }
    void setContent(const std::string& content) { _noteContent = content; }

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
    std::string _noteContent;  // TODO: get rid of it
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
    auto pager = _view->createComponent();
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
    if (notePtr) {
        _view->setContent(notePtr->getContent());
    } else {
        _view->clearContent();
    }
}

const ftxui::Component& Controller::component() const { return _model->getComponent(); }
ftxui::Element Controller::element() const { return _view->getElement(_model->getComponent()); }

}  // namespace linux::tui::preview
