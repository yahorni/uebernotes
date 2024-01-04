#pragma once

#include "linux/tui/menu_mvc.hpp"

#include <core/note.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui::note {

using Model = menu::Model<core::Note, core::NotesCache>;
using ViewBase = menu::View<true, core::BookID>;

class View : public ViewBase {
public:
    using ViewBase::View;

    ftxui::Element getElement(ftxui::Component& menu, int paneSize) const override;
};

using ControllerBase = menu::Controller<Model, View>;

class Controller : public ControllerBase {
public:
    using ControllerBase::Controller;

    void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) override;
    void configureComponent(ftxui::Component& menu, Communicator& communicator) override;
};

}  // namespace linux::tui::note
