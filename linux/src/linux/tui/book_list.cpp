#include "linux/tui/book_list.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <string>
#include <utility>

namespace linux::tui::book {

ftxui::Element View::getElement(ftxui::Component& menu, int paneSize) const {
    using namespace ftxui;  // NOLINT
    return vbox({
               hcenter(bold(text("Books"))),  // consider using "window"
               separator(),                   //
               menu->Render(),                //
           }) |
           borderDecorator(menu->Focused()) | size(WIDTH, EQUAL, paneSize);
}

void Controller::configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) {
    option.on_change = [&]() {
        communicator.cmdPush(Command::UpdateBook);
        Log::debug("Selected book: {}", *getSelectedItemID());
    };
    option.on_enter = [&]() {
        communicator.cmdPush(Command::UIEvent, ftxui::Event::ArrowRight);
    };
}

void Controller::configureComponent(ftxui::Component& menu, Communicator& communicator) {
    menu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto bookID = getSelectedItemID(); bookID) {
                message = std::format("Refreshed book: {}", *bookID);
            } else {
                message = "No book to refresh";
            }
            communicator.cmdPush(Command::RefreshBook);
            communicator.ntfPush(std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (sortByField(Sorter::Field::Name)) {
                communicator.cmdPush(Command::UpdateBook);
                communicator.ntfPush("Sort books by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (sortByField(Sorter::Field::CreationTime)) {
                communicator.cmdPush(Command::UpdateBook);
                communicator.ntfPush("Sort books by creation time");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            communicator.cmdPush(Command::UpdateBook);
            communicator.ntfPush(std::format("Ascending books sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            communicator.cmdPush(Command::UpdateBook);
            communicator.ntfPush(std::format("Show books ID: {}", enabled));
            return true;
        }
        return false;
    });
}

}  // namespace linux::tui::book
