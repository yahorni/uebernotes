#include "linux/tui/notes.hpp"

#include <string>

namespace linux::tui::notes {

Controller::Controller()
    : ControllerBase("Notes") {
    _refreshCMD = Command::RefreshNote;
    _sortCMD = Command::UpdateNote;
    _viewCMD = Command::UpdateNote;
}

void Controller::configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) {
    option.elements_postfix = [&] { return getItemsAmount() ? ftxui::linefiller('-') : ftxui::emptyElement(); };

    option.on_enter = [&]() {
        // TODO: add the same for Enter on PreviewPane
        communicator.cmd.push(Command::OpenEditor);
    };
}

}  // namespace linux::tui::notes
