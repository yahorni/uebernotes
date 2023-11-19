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

namespace {

using namespace ftxui;

const auto ignoreTabDecorator =
    IgnoreEvent([](Event event) { return event == Event::Tab || event == Event::TabReverse; });

auto borderDecorator(bool focused) {
    return focused ? borderStyled(BorderStyle::HEAVY) : borderStyled(BorderStyle::LIGHT);
}

class TUIContext {
public:
    // menu selections
    int selectedBookIdx = 0;
    int selectedNoteIdx = 0;
    std::map<core::BookID, int> storedNoteIndeces{};

    // preview pane
    int previewShift = 0;

    // status line
    std::string lastMessage = "Welcome to uebernotes";
};

class BookPane {
public:
    BookPane(core::Storage* storage, TUIContext* ctx, ScreenInteractive* screen)
        : _storage(storage),
          _ctx(ctx) {
        auto bookMenuOption = MenuOption::Vertical();
        // to select focused item immediately
        // FIXME: allow select without focusing
        bookMenuOption.focused_entry = &_ctx->selectedBookIdx;
        bookMenuOption.on_change = [&]() {
            auto bookPtr = getSelected();
            _ctx->selectedNoteIdx =
                (_ctx->storedNoteIndeces.count(bookPtr->id) ? _ctx->storedNoteIndeces.at(bookPtr->id) : 0);
            _ctx->previewShift = 0;

            Log::debug("Selected book: book={}, note={}", _ctx->selectedBookIdx, _ctx->selectedNoteIdx);
        };
        bookMenuOption.on_enter = [&]() { screen->PostEvent(Event::ArrowRight); };
        bookList = Menu(&bookNames, &_ctx->selectedBookIdx, bookMenuOption) | ignoreTabDecorator | FocusableWrapper();
    }

    // TODO: error-prone way. need to think how to redo it
    std::shared_ptr<core::BookInfo> getSelected() {
        const auto& books = _storage->getBookInfos();
        if (!books.size()) return nullptr;

        int i = 0;
        for (const auto& book : books) {
            if (i == _ctx->selectedBookIdx) return book;
            i++;
        }
        return nullptr;
    }

    void updateNames() {
        const auto& books = _storage->getBookInfos();

        // TODO: shrink_to_fit()
        bookNames.clear();
        bookNames.reserve(books.size());

        for (const auto& book : books) {
            bookNames.emplace_back(book->name);
        }
    }

    const Component& getComponent() const { return bookList; }

    Element getElement() const {
        return vbox({
                   hcenter(bold(text("Books"))),  // consider using "window"
                   separator(),                   //
                   bookList->Render(),            //
               }) |
               borderDecorator(bookList->Focused());
    }

private:
    core::Storage* _storage{nullptr};
    TUIContext* _ctx{nullptr};

    std::vector<std::string> bookNames;
    Component bookList;
};

class NotePane {
public:
    NotePane(core::Storage* storage, TUIContext* ctx, BookPane* bookPaneWrapper)
        : _storage(storage),
          _ctx(ctx) {
        auto noteMenuOption = MenuOption::Vertical();
        // to select focused item immediately
        // FIXME: allow select without focusing
        noteMenuOption.focused_entry = &_ctx->selectedNoteIdx;
        noteMenuOption.on_enter = [&]() {
            _ctx->lastMessage = std::format("Enter note: {}:{}", _ctx->selectedBookIdx, _ctx->selectedNoteIdx);
            // TODO: open editor
        };
        noteMenuOption.on_change = [&]() {
            auto bookPtr = bookPaneWrapper->getSelected();
            _ctx->storedNoteIndeces[bookPtr->id] = _ctx->selectedNoteIdx;
            _ctx->previewShift = 0;

            Log::debug("Selected note: book={}, note={}", _ctx->selectedBookIdx, _ctx->selectedNoteIdx);
        };
        // FIXME: show note name only before newline
        noteList = Menu(&noteNames, &_ctx->selectedNoteIdx, noteMenuOption) | ignoreTabDecorator | FocusableWrapper();
    }

    // TODO: error-prone way. need to think how to redo it
    std::shared_ptr<core::NoteInfo> getSelected(core::BookID bookID) {
        const auto& notes = _storage->getNoteInfosByBookID(bookID);
        if (!notes.size()) return nullptr;

        int i = 0;
        for (const auto& note : notes) {
            if (i == _ctx->selectedNoteIdx) return note;
            i++;
        }
        return nullptr;
    }

    void updateNames(core::BookID bookID) {
        const auto& notes = _storage->getNoteInfosByBookID(bookID);

        // TODO: shrink_to_fit()
        noteNames.clear();
        noteNames.reserve(notes.size());

        for (const auto& note : notes) {
            noteNames.emplace_back(note->getHeader());
        }
    }

    const Component& getComponent() const { return noteList; }

    Element getElement() const {
        return vbox({
                   hcenter(bold(text("Notes"))),  // consider using "window"
                   separator(),                   //
                   noteList->Render(),            //
               }) |
               borderDecorator(noteList->Focused());
    }

private:
    core::Storage* _storage{nullptr};
    TUIContext* _ctx{nullptr};

    std::vector<std::string> noteNames;
    Component noteList;
};

class PreviewPane {
public:
    explicit PreviewPane(TUIContext* ctx)
        : _ctx(ctx) {
        notePreview = Pager(_ctx->previewShift, noteContent);
    }

    void updateContent(std::shared_ptr<core::NoteInfo> notePtr) {
        if (notePtr)
            noteContent = notePtr->content;
        else
            noteContent.clear();
    }

    const Component& getComponent() const { return notePreview; }

    Element getElement() const {
        return vbox({
                   hcenter(bold(text("Preview"))),          // consider using "window"
                   separator(),                             //
                   notePreview->Render() | yflex | yframe,  //
               }) |
               xflex | borderDecorator(notePreview->Focused());
    }

private:
    TUIContext* _ctx{nullptr};

    std::string noteContent;  // TODO: optimize copying
    Component notePreview;
};

class StatusLine {
public:
    explicit StatusLine(TUIContext* ctx)
        : _ctx(ctx) {}

    Element getElement() const { return text(_ctx->lastMessage.c_str()); }

private:
    TUIContext* _ctx{nullptr};
};

}  // namespace

TUI::TUI(const core::AppContext& context)
    : _storage(context) {}

bool TUI::run() {
    using namespace ftxui;

    auto screen{ScreenInteractive::Fullscreen()};

    TUIContext ctx;

    BookPane bookPaneWrapper(&_storage, &ctx, &screen);
    NotePane notePaneWrapper(&_storage, &ctx, &bookPaneWrapper);
    PreviewPane previewPaneWrapper(&ctx);
    StatusLine statusLineWrapper(&ctx);

    auto container = Container::Horizontal({
        bookPaneWrapper.getComponent(),
        notePaneWrapper.getComponent(),
        previewPaneWrapper.getComponent(),
    });

    auto renderer = Renderer(container, [&] {
        int paneSize = Terminal::Size().dimx / 4;

        bookPaneWrapper.updateNames();
        auto bookPtr = bookPaneWrapper.getSelected();

        notePaneWrapper.updateNames(bookPtr->id);
        auto notePtr = notePaneWrapper.getSelected(bookPtr->id);

        previewPaneWrapper.updateContent(notePtr);

        return vbox({
            hbox({
                bookPaneWrapper.getElement() | size(WIDTH, EQUAL, paneSize),  //
                notePaneWrapper.getElement() | size(WIDTH, EQUAL, paneSize),  //
                previewPaneWrapper.getElement(),                              //
            }) | yflex,
            statusLineWrapper.getElement() | size(HEIGHT, EQUAL, 1),
        });
    });

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
