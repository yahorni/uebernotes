#include "linux/tui.hpp"
#include "linux/component.hpp"
#include "linux/logger.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

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
        noteNames.emplace_back(note->getHeader().substr(0, paneWidth));
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

    // preview data
    int previewShift = 0;
    std::string noteContent;  // TODO: optimize copying

    auto ignoreTabDecorator =
        IgnoreEvent([&](Event event) { return event == Event::Tab || event == Event::TabReverse; });

    // initialize book list pane
    auto&& bookMenuOption = MenuOption::Vertical();
    // to select focused item immediately
    bookMenuOption.focused_entry = &selectedBookIdx;
    bookMenuOption.on_change = [&]() {
        auto bookPtr = getSelectedBook(selectedBookIdx);
        selectedNoteIdx = (storedNoteIndeces.count(bookPtr->id) ? storedNoteIndeces.at(bookPtr->id) : 0);
        previewShift = 0;

        Log::debug("Selected book: book={}, note={}", selectedBookIdx, selectedNoteIdx);
    };
    auto bookList = Menu(&bookNames, &selectedBookIdx, bookMenuOption) | ignoreTabDecorator;

    // initialize note list pane
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
        previewShift = 0;

        Log::debug("Selected note: book={}, note={}", selectedBookIdx, selectedNoteIdx);
    };
    // FIXME: show note name only before newline
    auto noteList = Menu(&noteNames, &selectedNoteIdx, noteMenuOption) | ignoreTabDecorator;

    // initialize preview pane
    auto notePreview = Pager(previewShift, noteContent);

    // components container
    auto container = Container::Horizontal({
        bookList,
        noteList,
        notePreview,
    });

    // set up focus borders
    const auto focusBorder = borderStyled(BorderStyle::HEAVY);
    const auto unfocusBorder = borderStyled(BorderStyle::LIGHT);
    auto setBorder = [&](bool focused) { return focused ? focusBorder : unfocusBorder; };

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

        auto booksPane = vbox({
                             hcenter(bold(text("Books"))),  // consider using "window"
                             separator(),                   //
                             bookList->Render(),            //
                         }) |
                         size(WIDTH, EQUAL, paneSize) | setBorder(bookList->Focused());

        auto notesPane = vbox({
                             hcenter(bold(text("Notes"))),  // consider using "window"
                             separator(),                   //
                             noteList->Render(),            //
                         }) |
                         size(WIDTH, EQUAL, paneSize) | setBorder(noteList->Focused());

        auto previewPane = vbox({
                               hcenter(bold(text("Preview"))),          // consider using "window"
                               separator(),                             //
                               notePreview->Render() | yflex | yframe,  //
                           }) |
                           xflex | setBorder(notePreview->Focused());

        auto statusLine = Renderer([&] { return text(_lastMessage.c_str()) | size(HEIGHT, EQUAL, 1); });

        return vbox({
            hbox({
                booksPane,    //
                notesPane,    //
                previewPane,  //
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
