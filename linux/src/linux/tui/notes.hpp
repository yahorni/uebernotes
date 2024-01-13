#pragma once

#include "linux/tui/menu/controller.hpp"
#include "linux/tui/menu/model.hpp"
#include "linux/tui/menu/view.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui::notes {

using Model = menu::Model<core::NotePtr, core::NotesCache>;
using ControllerBase = menu::Controller<Model, menu::View>;

class Controller : public ControllerBase {
public:
    Controller();

    void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) override;
};

}  // namespace linux::tui::notes
