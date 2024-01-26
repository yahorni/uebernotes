#include "linux/tui/preview.hpp"

#include "ftxui/component.hpp"

#include <string>

namespace linux::tui::preview {

using ftxui::utils::OptionalCRef;

class Model : public mvc::Model {
public:
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
};

class View : public mvc::View {
public:
    ftxui::Component& createComponent(const OptionalCRef<std::string>& noteContent) {
        _component = ftxui::Pager(noteContent, _previewShift, _wrapLines);
        return _component;
    }

    void resetShift() { _previewShift = 0; }

    // UI element
    ftxui::Element element() const {
        using namespace ftxui;  // NOLINT
        return vbox({
                   hcenter(bold(text("Preview"))),         // consider using "window"
                   separator(),                            //
                   _component->Render() | yflex | yframe,  //
               }) |
               xflex | borderDecorator(_component->Focused());
    }

private:
    int _previewShift = 0;
    bool _wrapLines = true;
};

// Controller

Controller::Controller()
    : mvc::Controller("Preview"),
      _model(std::make_unique<Model>()),
      _view(std::make_unique<View>()) {}

// needed here for unique_ptr destruction
Controller::~Controller() = default;

void Controller::createComponent(Communicator& communicator) {
    auto& component{_view->createComponent(_model->getContent())};
    configureComponent(component, communicator);
}

void Controller::configureComponent(ftxui::Component& component, Communicator& communicator) {
    component |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Return) {
            communicator.cmd.push(Command::OpenEditor);
            communicator.ntf.push("TODO: Open editor");
            return true;
        }
        return false;
    });
}

void Controller::setNote(const core::NotePtr& notePtr) {
    _model->setNote(notePtr);
    _view->resetShift();
}

const ftxui::Component& Controller::component() const { return _view->component(); }
ftxui::Element Controller::element() const { return _view->element(); }

}  // namespace linux::tui::preview
