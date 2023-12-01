#include "linux/tui.hpp"

#include "linux/logger.hpp"
#include "linux/tui/book_list.hpp"
#include "linux/tui/event_queue.hpp"
#include "linux/tui/note_list.hpp"
#include "linux/tui/preview_pane.hpp"
#include "linux/tui/status_line.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

#include <string>

// ftxui/dom/elements.hpp - list of elements to create layouts
// ftxui/dom/component.hpp - interactive components to respond to events

// TODO: properly handle segfault

namespace linux {

TUI::TUI(const core::Config& config)
    : _storage(config),
      _eventQueue{},
      _bookList(&_storage, &_eventQueue),
      _noteList(&_storage, &_eventQueue),
      _previewPane(&_eventQueue),
      _statusLine(&_eventQueue) {}

bool TUI::run() {
    using namespace ftxui;

    auto screen{ScreenInteractive::Fullscreen()};
    // TODO: set minimal requirement for screen size

    auto container = Container::Horizontal({
        _bookList.getComponent(),
        _noteList.getComponent(),
        _previewPane.getComponent(),
    });

    updateComponents();

    auto renderer = Renderer(container, [&] {
        Log::debug("render");
        handleCommands(screen);

        int paneSize = Terminal::Size().dimx / 4;
        return vbox({
            hbox({
                _bookList.getElement() | size(WIDTH, EQUAL, paneSize),  //
                _noteList.getElement() | size(WIDTH, EQUAL, paneSize),  //
                _previewPane.getElement(),                              //
            }) | yflex,
            _statusLine.getElement() | size(HEIGHT, EQUAL, 1),
        });
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('R')) {
            _eventQueue.push(tui::Event::RefreshAll);
            return true;
        }
        return false;
    });

    screen.Loop(component);
    return true;
}

void TUI::updateComponents() {
    _bookList.updateItems();
    updateNoteComponents();
}

void TUI::updateNoteComponents(bool refresh) {
    if (auto bookID = _bookList.getSelectedBookID(); bookID) {
        _noteList.updateItems(*bookID, refresh);
        updateNotePreview(*bookID);
        if (refresh) {
            Log::info("Refreshed book: {}", *bookID);
        }
    } else {
        _previewPane.reset();
        if (refresh) {
            Log::warning("No book to refresh");
        }
    }
}

void TUI::updateNotePreview(core::BookID bookID) {
    if (auto notePtr = _noteList.getSelected(bookID); notePtr) {
        _previewPane.setContent(notePtr->content);
    } else {
        _previewPane.reset();
    }
}

void TUI::handleCommands(ftxui::ScreenInteractive& screen) {
    if (_eventQueue.empty()) {
        return;
    }

    auto [event, data] = _eventQueue.pop();

    switch (event) {
    case tui::Event::BookChanged: {
        updateNoteComponents();
        Log::debug("Book changed: book='{}'", *_bookList.getSelectedBookID());
    } break;
    case tui::Event::NoteChanged: {
        if (auto bookID = _bookList.getSelectedBookID(); bookID) {
            _noteList.cacheNoteIdx(*bookID);
            updateNotePreview(*bookID);
            Log::debug("Note changed: note='{}'", _noteList.getSelected(*bookID)->id);
        }
    } break;

    case tui::Event::UpdateStatusLine: {
        auto message{std::any_cast<std::string>(data)};
        _statusLine.setMessage(message);
    } break;
    case tui::Event::PostScreenEvent: {
        auto screenEvent{std::any_cast<ftxui::Event>(data)};
        screen.PostEvent(screenEvent);
    } break;

    case tui::Event::RefreshAll: {
        _storage.loadStorage();
        _bookList.reset();
        _noteList.reset();
        _previewPane.reset();
        updateComponents();
        Log::info("Refreshed books and notes");
    } break;
    case tui::Event::RefreshBook: {
        updateNoteComponents(true);
    } break;
    case tui::Event::RefreshNote: {
        if (auto bookID = _bookList.getSelectedBookID(); bookID) {
            updateNotePreview(*bookID);
            Log::info("Refreshed note");
        }
    } break;

    case tui::Event::OpenEditor: {
        if (auto bookID = _bookList.getSelectedBookID(); bookID) {
            _statusLine.setMessage(std::format("TODO: Open note: {}", _noteList.getSelected(*bookID)->id));
        }
    } break;
    default: {
        // TODO: add map with event names
        Log::warning("Unhandled event: {}", static_cast<int>(event));
    } break;
    }
}

}  // namespace linux
