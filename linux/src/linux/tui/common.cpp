#include "linux/tui/common.hpp"

namespace linux::tui {

ftxui::Decorator borderDecorator(bool focused) {
    return focused ? borderStyled(ftxui::BorderStyle::HEAVY) : borderStyled(ftxui::BorderStyle::LIGHT);
}

}  // namespace linux::tui
