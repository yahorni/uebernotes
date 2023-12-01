#include "linux/tui/book_list.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/event_queue.hpp"

#include <algorithm>

namespace linux {
namespace tui {

BookList::BookList(core::Storage* storage, EventQueue* eventQueue)
    : _storage(storage),
      _eventQueue(eventQueue) {
    auto bookMenuOption = ftxui::MenuOption::Vertical();

    // to select focused item immediately
    // FIXME: allow select without focusing
    bookMenuOption.focused_entry = &_selectedBookIdx;

    // set events
    bookMenuOption.on_change = [&]() { _eventQueue->push(Event::BookChanged); };
    bookMenuOption.on_enter = [&]() { _eventQueue->push(Event::PostScreenEvent, ftxui::Event::ArrowRight); };

    // set component
    _bookMenu = ftxui::Menu(&_bookNames, &_selectedBookIdx, bookMenuOption);
    _bookMenu |= ftxui::FocusableWrapper();

    // set keys
    _bookMenu |= ignoreTabDecorator;
    _bookMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            _eventQueue->push(Event::RefreshBook);
            return true;
        }
        return false;
    });
}

void BookList::reset() { _selectedBookIdx = 0; }

std::optional<core::BookID> BookList::getSelectedBookID() {
    const auto& books = _storage->getBookInfos();
    if (books.empty()) {
        return std::nullopt;
    }

    if (static_cast<size_t>(_selectedBookIdx) >= books.size()) {
        Log::warning("selected book index ({}) > amount of books ({})", _selectedBookIdx, books.size());
        return std::nullopt;
    }

    auto book = *std::next(books.begin(), _selectedBookIdx);
    return book->id;
}

void BookList::updateItems() {
    _selectedBookIdx = 0;

    const auto& books = _storage->getBookInfos();

    // TODO: shrink_to_fit()
    _bookNames.clear();
    _bookNames.reserve(books.size());

    for (const auto& book : books) {
        _bookNames.emplace_back(book->name);
    }

    // TODO: add sorting + selectedBookIdx
    // std::sort(_bookNames.begin(), _bookNames.end());
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

}  // namespace tui
}  // namespace linux
