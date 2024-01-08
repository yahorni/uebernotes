#include "linux/tui/books.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"

#include <string>
#include <utility>

namespace linux::tui::books {

ftxui::Element View::getElement(ftxui::Component& component, int width) const {
    using namespace ftxui;  // NOLINT
    return vbox({
               hcenter(bold(text("Books"))),  // consider using "window"
               separator(),                   //
               component->Render(),           //
           }) |
           borderDecorator(component->Focused()) | size(WIDTH, EQUAL, width);
}

void Controller::configureComponentOption(ftxui::MenuOption& option, Communicator& communicator) {
    option.on_change = [&]() {
        communicator.cmdPush(Command::UpdateBookWhenOrderKept);
        Log::debug("Selected book: {}", *getSelectedItemID());
    };
    option.on_enter = [&]() { communicator.cmdPush(Command::UIEvent, ftxui::Event::ArrowRight); };
}

void Controller::configureComponent(ftxui::Component& component, Communicator& communicator) {
    component |= ftxui::CatchEvent([&](ftxui::Event event) {
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
            auto field = toggleSortField();
            communicator.cmdPush(Command::UpdateBookWhenOrderChanged);
            switch (field) {
            case menu::Sorter::Field::CreationTime:
            case menu::Sorter::Field::UpdateTime:
                communicator.ntfPush("Sort books by creation time");
                break;
            case menu::Sorter::Field::Name:
                communicator.ntfPush("Sort books by name");
                break;
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            communicator.cmdPush(Command::UpdateBookWhenOrderChanged);
            communicator.ntfPush(std::format("Ascending books sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            communicator.cmdPush(Command::UpdateBookWhenOrderKept);
            communicator.ntfPush(std::format("Show books ID: {}", enabled));
            return true;
        }
        return false;
    });
}

}  // namespace linux::tui::books
