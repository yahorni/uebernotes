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

void Controller::configureComponentOption(ftxui::MenuOption& option) {
    option.on_change = [&]() {
        _eventQueue->push(Event::BookChanged, std::format("Selected book: {}", *getSelectedItemID()));
    };
    option.on_enter = [&]() { _eventQueue->push(Event::PostScreenEvent, "", ftxui::Event::ArrowRight); };
}

void Controller::configureComponent(ftxui::Component& menu) {
    // make always focusable
    menu |= ftxui::FocusableWrapper();

    // set keys
    menu |= ftxui::IgnoreEvents({ftxui::Event::Tab, ftxui::Event::TabReverse});
    menu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto bookID = getSelectedItemID(); bookID) {
                message = std::format("Refreshed book: {}", *bookID);
            } else {
                message = "No book to refresh";
            }
            _eventQueue->push(Event::RefreshBook, std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (sortByField(SortOptions::Field::Name)) {
                _eventQueue->push(Event::BookListUpdated, "Sort books by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (sortByField(SortOptions::Field::CreationTime)) {
                _eventQueue->push(Event::BookListUpdated, "Sort books by date");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool isAscending = toggleSortOrder();
            _eventQueue->push(Event::BookListUpdated, std::format("Ascending books sort order: {}", isAscending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = toggleShowID();
            _eventQueue->push(Event::BookListUpdated, std::format("Toggle books ID showing", enabled));
            return true;
        }
        return false;
    });

    menu |= ftxui::EventHandler({ftxui::Event::Character('j')});
    setComponent(menu);
}

}  // namespace linux::tui::book
