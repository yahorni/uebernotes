#include "linux/tui.hpp"
#include "linux/logger.hpp"

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

#include "ftxui/component/component.hpp"           // for CatchEvent, Renderer, operator|=
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive

// ftxui/dom/elements.hpp - list of elements to create layouts
// ftxui/dom/component.hpp - interactive components to respond to events

// TODO: properly handle segfault

namespace linux {

TUI::TUI(const core::AppContext& context)
    : _storage(context) {}

void TUI::_setStatusMessage(const std::string_view msg) {
    _lastMessage = msg;
    Log::debug("Set status: {}", _lastMessage);
}

std::vector<std::string> TUI::getBookNames() {
    const auto& books = _storage.getBookInfos();

    std::vector<std::string> bookNames;
    bookNames.reserve(books.size());

    for (const auto& book : books) {
        bookNames.emplace_back(book->name);
    }

    return bookNames;
}

std::vector<std::string> TUI::getNoteNames(core::BookID bookID, int paneWidth) {
    const auto& notes = _storage.getNoteInfosByBookID(bookID);
    if (!notes.size()) return {};

    std::vector<std::string> noteNames;
    noteNames.reserve(notes.size());
    for (const auto& note : notes) {
        noteNames.emplace_back(note->content.substr(0, paneWidth));
    }
    return noteNames;
}

// TODO: error-prone way. need to think how to redo it
std::shared_ptr<core::BookInfo> TUI::getSelectedBook(int selectedBook) {
    const auto& books = _storage.getBookInfos();
    if (!books.size()) return nullptr;

    int i = 0;
    for (const auto& book : books) {
        if (i == selectedBook) return book;
        i++;
    }
    return nullptr;
}

// TODO: error-prone way. need to think how to redo it
std::shared_ptr<core::NoteInfo> TUI::getSelectedNote(int selectedNoteIdx, core::BookID bookID) {
    const auto& notes = _storage.getNoteInfosByBookID(bookID);
    if (!notes.size()) return nullptr;

    int i = 0;
    for (const auto& note : notes) {
        if (i == selectedNoteIdx) return note;

        i++;
    }
    return nullptr;
}

bool TUI::run() {
    using namespace ftxui;

    // book data
    std::vector<std::string> bookNames;
    int selectedBookIdx = 0;

    // note data
    std::vector<std::string> noteNames;
    std::map<core::BookID, int> storedNoteIndeces;
    int selectedNoteIdx = 0;

    auto&& bookMenuOption = MenuOption::Vertical();
    // to select focused item immediately
    bookMenuOption.focused_entry = &selectedBookIdx;
    bookMenuOption.on_change = [&]() {
        auto bookPtr = getSelectedBook(selectedBookIdx);
        if (storedNoteIndeces.count(bookPtr->id)) {
            selectedNoteIdx = storedNoteIndeces.at(bookPtr->id);
        } else {
            selectedNoteIdx = 0;
        }
        Log::debug("Changed book: book={}, note={}", selectedBookIdx, selectedNoteIdx);
    };
    auto bookList = Menu(&bookNames, &selectedBookIdx, bookMenuOption);

    auto&& noteMenuOption = MenuOption::Vertical();
     // to select focused item immediately
    noteMenuOption.focused_entry = &selectedNoteIdx;
    noteMenuOption.on_enter = [&]() {
        _setStatusMessage(std::format("Enter note: {}:{}", selectedBookIdx, selectedNoteIdx));
        // TODO: open editor
    };
    noteMenuOption.on_change = [&]() {
        auto bookPtr = getSelectedBook(selectedBookIdx);
        storedNoteIndeces[bookPtr->id] = selectedNoteIdx;
        Log::debug("Changed note: book={}, note={}", selectedBookIdx, selectedNoteIdx);
    };
    auto noteList = Menu(&noteNames, &selectedNoteIdx, noteMenuOption);

    // TODO: optimize copying
    std::string noteContent;
    auto notePreview = Renderer([&] { return hflow(paragraph(noteContent)); });

    auto container = Container::Horizontal({
        bookList,
        noteList,
        notePreview,
    });

    auto renderer = Renderer(container, [&] {
        int paneSize = Terminal::Size().dimx / 4;

        bookNames = getBookNames();
        auto bookPtr = getSelectedBook(selectedBookIdx);

        noteNames = getNoteNames(bookPtr->id, paneSize);
        auto notePtr = getSelectedNote(selectedNoteIdx, bookPtr->id);
        if (notePtr) {
            noteContent = notePtr->content;
        } else {
            noteContent.clear();
        }

        auto booksPane = Renderer([&] {
            return vbox({
                       hcenter(bold(text("Books"))),  //
                       separator(),                   //
                       bookList->Render(),            //
                   }) |
                   size(WIDTH, EQUAL, paneSize) | border;
        });

        auto notesPane = Renderer([&] {
            return vbox({
                       hcenter(bold(text("Notes"))),  //
                       separator(),                   //
                       noteList->Render(),            //
                   }) |
                   size(WIDTH, EQUAL, Terminal::Size().dimx / 4) | border;
        });

        auto previewPane = Renderer([&] {
            return vbox({
                       hcenter(bold(text("Note preview"))),  //
                       separator(),                          //
                       notePreview->Render(),                //
                   }) |
                   xflex | border;
        });

        auto statusLine = Renderer([&] { return text(_lastMessage.c_str()) | size(HEIGHT, EQUAL, 1); });

        return vbox({
            hbox({
                booksPane->Render(),    //
                notesPane->Render(),    //
                previewPane->Render(),  //
            }) | yflex,
            statusLine->Render(),
        });
    });

    auto screen = ScreenInteractive::Fullscreen();

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        return false;
    });

    screen.Loop(component);
    return true;
}

}  // namespace linux
