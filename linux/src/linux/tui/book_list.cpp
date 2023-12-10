#include "linux/tui/book_list.hpp"

#include "ftxui/component.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/event_queue.hpp"

namespace linux::tui {

BookList::BookList(core::Storage* storage, EventQueue* eventQueue)
    : _storage(storage),
      _eventQueue(eventQueue) {
    auto bookMenuOption = ftxui::MenuOption::Vertical();

    // set events
    bookMenuOption.on_change = [&]() { _eventQueue->push(Event::BookChanged); };
    bookMenuOption.on_enter = [&]() { _eventQueue->push(Event::PostScreenEvent, ftxui::Event::ArrowRight); };

    // set component
    _bookMenu = _menuController.createMenu(bookMenuOption);
    _bookMenu |= ftxui::FocusableWrapper();

    // set keys
    _bookMenu |= ignoreTabDecorator;
    _bookMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            _eventQueue->push(Event::RefreshBook);
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (_menuController.setSortType(SortType::Ascending)) {
                _eventQueue->push(Event::RefreshAll);
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (_menuController.setSortType(SortType::Descending)) {
                _eventQueue->push(Event::RefreshAll);
            }
            return true;
        } else if (event == ftxui::Event::Character('t')) {
            if (_menuController.setSortType(SortType::CreationTime)) {
                _eventQueue->push(Event::RefreshAll);
            }
            return true;
        }
        return false;
    });
}

void BookList::reset() { _menuController.resetIndex(); }

std::optional<core::BookID> BookList::getSelectedID() const { return _menuController.getSelectedItemID(); }

void BookList::updateItems() {
    const auto& books = _storage->getBookInfos();
    _menuController.reloadItems(books);
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
