#pragma once

#include "linux/tui/communicator.hpp"
#include "linux/utils/noncopyable.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

#include <memory>

namespace linux::tui::preview {

class Model;
class View;

class Controller : private utils::NonCopyable {
public:
    Controller();
    ~Controller();

    void createComponent(Communicator& communicator);

    void setNote(const core::NotePtr& notePtr);

    const ftxui::Component& component() const;
    ftxui::Element element() const;

private:
    void configureComponent(ftxui::Component& component, Communicator& communicator);

    std::unique_ptr<Model> _model;
    std::unique_ptr<View> _view;
};

}  // namespace linux::tui::preview
