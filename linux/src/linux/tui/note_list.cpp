#include "linux/tui/note_list.hpp"

#include "ftxui/component.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/event_queue.hpp"

#include <core/types.hpp>

namespace linux::tui {

NoteList::NoteList(core::Storage* storage, EventQueue* eventQueue)
    : _storage(storage),
      _eventQueue(eventQueue) {
    auto noteMenuOption = ftxui::MenuOption::Vertical();

    // set events
    noteMenuOption.on_enter = [&]() { _eventQueue->push(Event::OpenEditor, _menuController.getSelectedItemID()); };
    noteMenuOption.on_change = [&]() { _eventQueue->push(Event::NoteChanged, _menuController.getSelectedItemID()); };

    // set component
    _noteMenu = _menuController.createMenu(noteMenuOption);
    _noteMenu |= ftxui::FocusableWrapper();

    // set keys
    _noteMenu |= ignoreTabDecorator;
    _noteMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            _eventQueue->push(Event::RefreshNote);
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (_menuController.setSortType(SortType::Ascending)) {
                _eventQueue->push(Event::RefreshBook);
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (_menuController.setSortType(SortType::Descending)) {
                _eventQueue->push(Event::RefreshBook);
            }
            return true;
        } else if (event == ftxui::Event::Character('t')) {
            if (_menuController.setSortType(SortType::CreationTime)) {
                _eventQueue->push(Event::RefreshBook);
            }
            return true;
        }
        return false;
    });
}

void NoteList::reset() { _menuController.resetIndex(); }

void NoteList::cacheIndex(core::BookID bookID) { _menuController.cacheIndex(bookID); }

std::shared_ptr<core::Note> NoteList::getSelectedItem() const { return _menuController.getSelectedItem(); }

std::optional<core::NoteID> NoteList::getSelectedID() const { return _menuController.getSelectedItemID(); }

void NoteList::updateItems(core::BookID bookID, bool refresh) {
    if (refresh) {
        _menuController.resetIndex(bookID);
    }

    const auto& notes = _storage->getNotesByBookID(bookID, refresh);
    _menuController.reloadItems(notes);

    if (!refresh) {
        _menuController.useCachedIndex(bookID);
    }
}

const ftxui::Component& NoteList::getComponent() const { return _noteMenu; }

ftxui::Element NoteList::getElement() const {
    using namespace ftxui;
    return vbox({
               hcenter(bold(text("Notes"))),  // consider using "window"
               separator(),                   //
               _noteMenu->Render(),           //
           }) |
           borderDecorator(_noteMenu->Focused());
}

}  // namespace linux::tui
