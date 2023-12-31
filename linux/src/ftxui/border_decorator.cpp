#include "ftxui/component.hpp"

namespace ftxui {

Decorator borderDecorator(bool focused) {
    return focused ? borderStyled(ftxui::BorderStyle::HEAVY) : borderStyled(ftxui::BorderStyle::LIGHT);
}

}  // namespace ftxui
