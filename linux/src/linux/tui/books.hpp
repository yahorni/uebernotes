#pragma once

#include "linux/tui/menu/mvc.hpp"

#include <core/book.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui::books {

using Model = menu::Model<core::Book, core::BooksCache>;
using ViewBase = menu::View<>;

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

}  // namespace linux::tui::books
