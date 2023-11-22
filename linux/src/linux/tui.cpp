#include "linux/tui.hpp"

#include "ftxui/component.hpp"
#include "linux/logger.hpp"
#include "linux/tui/book_list.hpp"
#include "linux/tui/note_list.hpp"
#include "linux/tui/preview_pane.hpp"
#include "linux/tui/status_line.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

// ftxui/dom/elements.hpp - list of elements to create layouts
// ftxui/dom/component.hpp - interactive components to respond to events

// TODO: properly handle segfault

namespace linux {

TUI::TUI(const core::Config& config)
    : _storage(config) {}

bool TUI::run() {
    using namespace ftxui;

    auto screen{ScreenInteractive::Fullscreen()};
    // TODO: set minimal requirement for screen size

    tui::Context ctx;

    tui::BookList bookList(&_storage, &ctx, screen);
    tui::NoteList noteList(&_storage, &ctx, bookList);
    tui::PreviewPane previewPane(&ctx);
    tui::StatusLine statusLine(&ctx);

    auto container = Container::Horizontal({
        bookList.getComponent(),
        noteList.getComponent(),
        previewPane.getComponent(),
    });

    auto renderer = Renderer(container, [&] {
        int paneSize = Terminal::Size().dimx / 4;

        bookList.updateItems();
        auto bookPtr = bookList.getSelected();

        noteList.updateItems(bookPtr->id);
        auto notePtr = noteList.getSelected(bookPtr->id);

        previewPane.updateContent(notePtr);

        return vbox({
            hbox({
                bookList.getElement() | size(WIDTH, EQUAL, paneSize),  //
                noteList.getElement() | size(WIDTH, EQUAL, paneSize),  //
                previewPane.getElement(),                              //
            }) | yflex,
            statusLine.getElement() | size(HEIGHT, EQUAL, 1),
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
