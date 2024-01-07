#include "linux/tui/notes.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <core/types.hpp>

#include <string>
#include <utility>

namespace linux::tui::notes {

ftxui::Element View::getElement(ftxui::Component& component, int paneSize) const {
    using namespace ftxui;  // NOLINT
    return vbox({
               hcenter(bold(text("Notes"))),  // consider using "window"
               separator(),                   //
               component->Render(),           //
           }) |
           borderDecorator(component->Focused()) | size(WIDTH, EQUAL, paneSize);
}

void Controller::configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) {
    option.elements_postfix = [&] { return getItems().size() ? ftxui::linefiller('-') : ftxui::emptyElement(); };

    option.on_change = [&]() {
        communicator.cmdPush(Command::UpdateNote);
        Log::debug("Selected note: {}", *getSelectedItemID());
    };
    option.on_enter = [&]() {
        // TODO: add the same for Enter on PreviewPane
        if (getItems().size()) {
            communicator.cmdPush(Command::OpenEditor);
            communicator.ntfPush("TODO: Open editor");
        } else {
            communicator.ntfPush("No selected note to open");
        }
    };
}

void Controller::configureComponent(ftxui::Component& component, Communicator& communicator) {
    component |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto noteID = getSelectedItemID(); noteID) {
                message = std::format("Refreshed note: {}", *noteID);
            } else {
                message = "No note to refresh";
            }
            communicator.cmdPush(Command::RefreshNote);
            communicator.ntfPush(std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            auto field = toggleSortField();
            communicator.cmdPush(Command::UpdateNote);
            switch (field) {
            case menu::Sorter::Field::CreationTime:
            case menu::Sorter::Field::UpdateTime:
                communicator.ntfPush("Sort notes by creation time");
                break;
            case menu::Sorter::Field::Name:
                communicator.ntfPush("Sort notes by name");
                break;
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            communicator.cmdPush(Command::UpdateNote);
            communicator.ntfPush(std::format("Ascending notes sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            communicator.cmdPush(Command::UpdateNote);
            communicator.ntfPush(std::format("Show notes ID: {}", enabled));
            return true;
        }
        return false;
    });
}

}  // namespace linux::tui::notes
