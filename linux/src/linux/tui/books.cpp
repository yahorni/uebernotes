#include "linux/tui/books.hpp"

#include <string>

namespace linux::tui::books {

Controller::Controller()
    : ControllerBase("Books") {
    _refreshCMD = Command::RefreshBook;
    _sortCMD = Command::UpdateBookWhenOrderChanged;
    _viewCMD = Command::UpdateBookWhenOrderKept;
}

void Controller::configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) {
    option.on_enter = [&]() { communicator.ui.push(ftxui::Event::ArrowRight); };
}

}  // namespace linux::tui::books
