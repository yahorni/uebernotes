#pragma once

#include "linux/tui/menu_mvc.hpp"

#include <core/book.hpp>

#include <ftxui/component/component.hpp>

namespace linux::tui::book {

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

    void configureComponentOption(ftxui::MenuOption& option) override;
    void configureComponent(ftxui::Component& menu) override;
};

}  // namespace linux::tui::book
