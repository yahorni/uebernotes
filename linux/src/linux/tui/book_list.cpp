#include "linux/tui/book_list.hpp"

#include "ftxui/component.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/event_queue.hpp"

#include <string>
#include <utility>

namespace linux::tui {

BookList::BookList(core::Storage* storage, EventQueue* eventQueue)
    : _storage(storage),
      _eventQueue(eventQueue) {
    auto bookMenuOption = ftxui::MenuOption::Vertical();

    // set events
    bookMenuOption.on_change = [&]() {
        _eventQueue->push(Event::BookChanged, std::format("Selected book: {}", *_menuController.getSelectedItemID()));
    };
    bookMenuOption.on_enter = [&]() { _eventQueue->push(Event::PostScreenEvent, "", ftxui::Event::ArrowRight); };

    // set component
    _bookMenu = _menuController.createMenu(bookMenuOption);
    _bookMenu |= ftxui::FocusableWrapper();

    // set keys
    _bookMenu |= ignoreTabDecorator;
    _bookMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto bookID = getSelectedID(); bookID) {
                message = std::format("Refreshed book: {}", *bookID);
            } else {
                message = "No book to refresh";
            }
            _eventQueue->push(Event::RefreshBook, std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (_menuController.setSortType(SortField::Name)) {
                _menuController.sortItems();
                _eventQueue->push(Event::BookListUpdated, "Sort books by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (_menuController.setSortType(SortField::CreationTime)) {
                _menuController.sortItems();
                _eventQueue->push(Event::BookListUpdated, "Sort books by date");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool ascending = _menuController.toggleSortOrder();
            _menuController.sortItems();
            _eventQueue->push(Event::BookListUpdated, std::format("Ascending books sort order: {}", ascending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = _menuController.toggleShowID();
            _eventQueue->push(Event::BookListUpdated, std::format("Toggle books ID showing", enabled));
            return true;
        }
        return false;
    });
}

void BookList::reset() { _menuController.resetIndex(); }

std::optional<core::BookID> BookList::getSelectedID() const { return _menuController.getSelectedItemID(); }

void BookList::reloadItems() {
    const auto& books = _storage->getBooks();
    _menuController.setItems(books);
}

const ftxui::Component& BookList::getComponent() const { return _bookMenu; }

ftxui::Element BookList::getElement() const {
    using namespace ftxui;
    return vbox({
               hcenter(bold(text("Books"))),  // consider using "window"
               separator(),                   //
               _bookMenu->Render(),           //
           }) |
           borderDecorator(_bookMenu->Focused());
}

}  // namespace linux::tui
