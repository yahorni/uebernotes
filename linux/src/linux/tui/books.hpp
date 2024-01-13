#pragma once

#include "linux/tui/menu/controller.hpp"
#include "linux/tui/menu/model.hpp"
#include "linux/tui/menu/view.hpp"

#include <core/book.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui::books {

using Model = menu::Model<core::BookPtr, core::BooksCache>;
using ControllerBase = menu::Controller<Model, menu::View>;

class Controller : public ControllerBase {
public:
    Controller();

    void configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) override;
};

}  // namespace linux::tui::books
