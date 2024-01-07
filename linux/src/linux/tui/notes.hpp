#pragma once

#include "linux/tui/menu/model.hpp"
#include "linux/tui/menu/view.hpp"
#include "linux/tui/menu/controller.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui::notes {

using Model = menu::Model<core::Note, core::NotesCache>;

// TODO: try CRTP instead of inheritance
class View : public menu::View {
public:
    using menu::View::View;

    ftxui::Element getElement(ftxui::Component& menu, int paneSize) const override;
};

using ControllerBase = menu::Controller<Model, View>;

class Controller : public ControllerBase {
public:
    using ControllerBase::Controller;

    void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) override;
    void configureComponent(ftxui::Component& menu, Communicator& communicator) override;
};

}  // namespace linux::tui::notes
