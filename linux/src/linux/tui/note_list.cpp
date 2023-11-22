#include "linux/tui/note_list.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/common.hpp"

namespace linux::tui {

NoteList::NoteList(core::Storage* storage, Context* ctx, BookList& bookList)
    : _storage(storage),
      _ctx(ctx) {
    auto noteMenuOption = ftxui::MenuOption::Vertical();
    // to select focused item immediately
    // FIXME: allow select without focusing
    noteMenuOption.focused_entry = &_ctx->selectedNoteIdx;
    noteMenuOption.on_enter = [&]() {
        _ctx->lastMessage = std::format("Enter note: {}:{}", _ctx->selectedBookIdx, _ctx->selectedNoteIdx);
        // TODO: open editor
    };
    noteMenuOption.on_change = [&]() {
        auto bookPtr = bookList.getSelected();
        _ctx->storedNoteIndeces[bookPtr->id] = _ctx->selectedNoteIdx;
        _ctx->previewShift = 0;

        Log::debug("Selected note: book={}, note={}", _ctx->selectedBookIdx, _ctx->selectedNoteIdx);
    };
    // FIXME: show note name only before newline
    noteMenu = ftxui::Menu(&noteNames, &_ctx->selectedNoteIdx, noteMenuOption) | ignoreTabDecorator |
               ftxui::FocusableWrapper();
}

// TODO: error-prone way. need to think how to redo it
std::shared_ptr<core::NoteInfo> NoteList::getSelected(core::BookID bookID) {
    const auto& notes = _storage->getNoteInfosByBookID(bookID);
    if (!notes.size()) return nullptr;

    int i = 0;
    for (const auto& note : notes) {
        if (i == _ctx->selectedNoteIdx) return note;
        i++;
    }
    return nullptr;
}

void NoteList::updateItems(core::BookID bookID) {
    const auto& notes = _storage->getNoteInfosByBookID(bookID);

    // TODO: shrink_to_fit()
    noteNames.clear();
    noteNames.reserve(notes.size());
    for (const auto& note : notes) {
        noteNames.emplace_back(note->getHeader());
    }
}

const ftxui::Component& NoteList::getComponent() const { return noteMenu; }

ftxui::Element NoteList::getElement() const {
    using namespace ftxui;
    return vbox({
               hcenter(bold(text("Notes"))),  // consider using "window"
               separator(),                   //
               noteMenu->Render(),            //
           }) |
           borderDecorator(noteMenu->Focused());
}

}  // namespace linux::tui
