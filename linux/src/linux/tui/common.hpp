#pragma once

#include "ftxui/component.hpp"

namespace linux::tui {

const auto ignoreTabDecorator = ftxui::IgnoreEvent(
    [](ftxui::Event event) { return event == ftxui::Event::Tab || event == ftxui::Event::TabReverse; });

ftxui::Decorator borderDecorator(bool focused);

}  // namespace linux::tui
