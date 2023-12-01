#include "linux/tui/note_list.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/common.hpp"
#include "linux/tui/event_queue.hpp"

#include <core/types.hpp>

#include <algorithm>

namespace linux::tui {

NoteList::NoteList(core::Storage* storage, EventQueue* eventQueue)
    : _storage(storage),
      _eventQueue(eventQueue) {
    auto noteMenuOption = ftxui::MenuOption::Vertical();

    // to select focused item immediately
    // FIXME: allow select without focusing
    noteMenuOption.focused_entry = &_selectedNoteIdx;

    // set events
    noteMenuOption.on_enter = [&]() { _eventQueue->push(Event::OpenEditor /* pass note index */); };
    noteMenuOption.on_change = [&]() { _eventQueue->push(Event::NoteChanged, _selectedNoteIdx); };

    // set component
    _noteMenu = ftxui::Menu(&_noteNames, &_selectedNoteIdx, noteMenuOption);
    _noteMenu |= ftxui::FocusableWrapper();

    // set keys
    _noteMenu |= ignoreTabDecorator;
    _noteMenu |= ftxui::CatchEvent([&](ftxui::Event event) {
        if (event == ftxui::Event::Character('r')) {
            _eventQueue->push(Event::RefreshNote);
            return true;
        }
        return false;
    });
}

void NoteList::reset() {
    _selectedNoteIdx = 0;
    _storedNoteIndeces.clear();
}

void NoteList::cacheNoteIdx(core::BookID bookID) { _storedNoteIndeces[bookID] = _selectedNoteIdx; }

std::shared_ptr<core::NoteInfo> NoteList::getSelected(core::BookID bookID, bool refresh) {
    const auto& notes = _storage->getNoteInfosByBookID(bookID, refresh);
    if (!notes.size()) {
        return nullptr;
    }

    if (static_cast<size_t>(_selectedNoteIdx) >= notes.size()) {
        Log::warning("selected note index ({}) > amount of notes ({})", _selectedNoteIdx, notes.size());
        return nullptr;
    }

    return *std::next(notes.begin(), _selectedNoteIdx);
}

void NoteList::updateItems(core::BookID bookID, bool refresh) {
    if (refresh) {
        _selectedNoteIdx = 0;
        _storedNoteIndeces[bookID] = 0;
    }

    const auto& notes = _storage->getNoteInfosByBookID(bookID, refresh);

    // TODO: shrink_to_fit()
    _noteNames.clear();
    _noteNames.reserve(notes.size());
    for (const auto& note : notes) {
        _noteNames.emplace_back(note->getHeader());
    }

    // add sorting + selectedBookIdx
    // std::sort(_noteNames.begin(), _noteNames.end());

    if (!refresh) {
        _selectedNoteIdx = _storedNoteIndeces.count(bookID) ? _storedNoteIndeces.at(bookID) : 0;
    }
}

const ftxui::Component& NoteList::getComponent() const { return _noteMenu; }

ftxui::Element NoteList::getElement() const {
    using namespace ftxui;
    return vbox({
               hcenter(bold(text("Notes"))),  // consider using "window"
               separator(),                   //
               _noteMenu->Render(),            //
           }) |
           borderDecorator(_noteMenu->Focused());
}

}  // namespace linux::tui
