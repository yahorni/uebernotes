#pragma once

#include "linux/tui/communicator.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

#include <memory>

namespace linux::tui::preview {

class Model;
class View;

class Controller {
public:
    Controller();
    Controller(const Controller&) = delete;
    Controller(Controller&&) = delete;
    Controller& operator=(const Controller&) = delete;
    Controller& operator=(Controller&&) = delete;
    ~Controller();

    void createComponent(Communicator& communicator);
    void configureComponent(ftxui::Component& component, Communicator& communicator);

    void setNote(const core::NotePtr& notePtr);

    const ftxui::Component& component() const;
    ftxui::Element element() const;

private:
    std::unique_ptr<Model> _model;
    std::unique_ptr<View> _view;
};

}  // namespace linux::tui::preview
