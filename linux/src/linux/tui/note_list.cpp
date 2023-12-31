#include "linux/tui/note_list.hpp"

#include "ftxui/component.hpp"
#include "linux/tui/event_queue.hpp"

#include <core/types.hpp>

#include <string>
#include <utility>

namespace linux::tui {

NoteList::NoteList(core::Storage* storage, EventQueue* eventQueue)
    : _storage(storage),
      _eventQueue(eventQueue),
      _menuController(eventQueue, true) {
    auto noteMenuOption = ftxui::MenuOption::Vertical();
    noteMenuOption.elements_postfix = [this] {
        return _menuController.getItemsAmount() ? ftxui::linefiller('-') : ftxui::emptyElement();
    };

    // set events
    noteMenuOption.on_change = [&]() {
        _eventQueue->push(Event::NoteChanged, std::format("Selected note: {}", *_menuController.getSelectedItemID()));
    };
    noteMenuOption.on_enter = [&]() {
        _eventQueue->push(Event::OpenEditor, "TODO: Open editor", _menuController.getSelectedItemID());
    };

    // set component
    _noteMenu = _menuController.createMenu(noteMenuOption);
    _noteMenu |= ftxui::FocusableWrapper();

    // set keys
    _noteMenu |= ftxui::IgnoreEvents({ftxui::Event::Tab, ftxui::Event::TabReverse});
    _noteMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            std::string message;
            if (auto noteID = getSelectedID(); noteID) {
                message = std::format("Refreshed note: {}", *noteID);
            } else {
                message = "No note to refresh";
            }
            _eventQueue->push(Event::RefreshNote, std::move(message));
            return true;
        } else if (event == ftxui::Event::Character('s')) {
            if (_menuController.setSortType(SortField::Name)) {
                _menuController.sortItems();
                _eventQueue->push(Event::NoteListUpdated, "Sort notes by name");
            }
            return true;
        } else if (event == ftxui::Event::Character('S')) {
            if (_menuController.setSortType(SortField::CreationTime)) {
                _menuController.sortItems();
                _eventQueue->push(Event::NoteListUpdated, "Sort notes by date");
            }
            return true;
        } else if (event == ftxui::Event::Character('o')) {
            bool ascending = _menuController.toggleSortOrder();
            _menuController.sortItems();
            _eventQueue->push(Event::NoteListUpdated, std::format("Ascending notes sort order: {}", ascending));
            return true;
        } else if (event == ftxui::Event::Character('i')) {
            bool enabled = _menuController.toggleShowID();
            _eventQueue->push(Event::NoteListUpdated, std::format("Show notes ID: {}", enabled));
            return true;
        }
        return false;
    });

    _noteMenu |= ftxui::EventHandler({ftxui::Event::Character('j')});
}

void NoteList::reset() { _menuController.resetIndex(); }

void NoteList::cacheIndex(core::BookID bookID) { _menuController.cacheIndex(bookID); }

std::shared_ptr<core::Note> NoteList::getSelectedItem() const { return _menuController.getSelectedItem(); }

std::optional<core::NoteID> NoteList::getSelectedID() const { return _menuController.getSelectedItemID(); }

void NoteList::reloadItems(core::BookID bookID, bool useCached) {
    if (!useCached) {
        _menuController.resetIndex(bookID);
    }

    const auto& notes = _storage->getNotesByBookID(bookID, useCached);
    _menuController.setItems(notes);

    if (useCached) {
        _menuController.useCachedIndex(bookID);
    }
}

const ftxui::Component& NoteList::getComponent() const { return _noteMenu; }

ftxui::Element NoteList::getElement(int paneSize) const {
    using namespace ftxui;
    return vbox({
               hcenter(bold(text("Notes"))),  // consider using "window"
               separator(),                   //
               _noteMenu->Render(),           //
           }) |
           borderDecorator(_noteMenu->Focused()) | size(WIDTH, EQUAL, paneSize);
}

}  // namespace linux::tui
