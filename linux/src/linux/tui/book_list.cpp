#include "linux/tui/book_list.hpp"

#include "ftxui/component.hpp"
#include "linux/tui/event_queue.hpp"

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

void Controller::configureComponentOption(ftxui::MenuOption& option, EventQueue& eventQueue) {
    option.on_change = [&]() {
        eventQueue.push(Event::BookChanged, std::format("Selected book: {}", *getSelectedItemID()));
    };
    option.on_enter = [&]() { eventQueue.push(Event::PostScreenEvent, "", ftxui::Event::ArrowRight); };
}

void Controller::configureComponent(ftxui::Component& menu, EventQueue& eventQueue) {
    menu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto bookID = getSelectedItemID(); bookID) {
                message = std::format("Refreshed book: {}", *bookID);
            } else {
                message = "No book to refresh";
            }
            eventQueue.push(Event::RefreshBook, std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (sortByField(Sorter::Field::Name)) {
                eventQueue.push(Event::BookListUpdated, "Sort books by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (sortByField(Sorter::Field::CreationTime)) {
                eventQueue.push(Event::BookListUpdated, "Sort books by date");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            eventQueue.push(Event::BookListUpdated, std::format("Ascending books sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            eventQueue.push(Event::BookListUpdated, std::format("Toggle books ID showing", enabled));
            return true;
        }
        return false;
    });
}

}  // namespace linux::tui::book
