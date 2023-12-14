#include "linux/tui.hpp"

#include "linux/logger.hpp"

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
      _bottomLine(&_eventQueue) {}

bool TUI::run() {
    using namespace ftxui;

    auto screen{ScreenInteractive::Fullscreen()};
    // TODO: set minimal requirement for screen size

    auto container = Container::Vertical({Container::Horizontal({
                                              _bookList.getComponent(),
                                              _noteList.getComponent(),
                                              _previewPane.getComponent(),
                                          }),
                                          _bottomLine.getComponent()});

    initComponents();

    auto renderer = Renderer(container, [&] {
        Log::debug("render, events: {}", _eventQueue.size());
        handleCommands(screen);

        int paneSize = Terminal::Size().dimx / 4;
        return vbox({
            hbox({
                _bookList.getElement() | size(WIDTH, EQUAL, paneSize),  //
                _noteList.getElement() | size(WIDTH, EQUAL, paneSize),  //
                _previewPane.getElement(),                              //
            }) | yflex,
            _bottomLine.getElement() | size(HEIGHT, EQUAL, 1),
        });
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('R')) {
            _eventQueue.push(tui::Event::RefreshAll);
            return true;
        } else if (event == Event::Character('/')) {
            _bottomLine.setMode(tui::BottomLine::Mode::Search);
            _bottomLine.getComponent()->TakeFocus();
            return true;
        } else if (event == Event::Character(':')) {
            _bottomLine.setMode(tui::BottomLine::Mode::Command);
            _bottomLine.getComponent()->TakeFocus();
            return true;
        }
        return false;
    });

    screen.Loop(component);
    return true;
}

void TUI::initComponents() {
    _bookList.reset();
    _noteList.reset();
    _previewPane.reset();

    _bookList.reloadItems();
    updateNotesAndPreview();
}

void TUI::updateNotesAndPreview(bool useCached) {
    auto bookID = _bookList.getSelectedID();
    _noteList.reloadItems(*bookID, useCached);
    redrawNotePreview();
}

void TUI::redrawNotePreview() {
    if (auto notePtr = _noteList.getSelectedItem(); notePtr) {
        _previewPane.setContent(notePtr->content);
    } else {
        _previewPane.reset();
    }
}

void TUI::handleCommands(ftxui::ScreenInteractive& screen) {
    if (_eventQueue.empty()) {
        return;
    }

    auto [event, message, data] = _eventQueue.pop();

    if (!message.empty()) {
        Log::debug("Event message: {}", message);
    }

    switch (event) {
    case tui::Event::BookChanged: {
        updateNotesAndPreview();
    } break;
    case tui::Event::NoteChanged: {
        if (auto bookID = _bookList.getSelectedID(); bookID) {
            _noteList.cacheIndex(*bookID);
            redrawNotePreview();
        }
    } break;

    case tui::Event::BookListUpdated: {
        updateNotesAndPreview();
    } break;
    case tui::Event::NoteListUpdated: {
        redrawNotePreview();
    } break;

    case tui::Event::UpdateStatus: {
        auto message{std::any_cast<std::string>(data)};
        _bottomLine.setMessage(message);
    } break;
    case tui::Event::PostScreenEvent: {
        auto screenEvent{std::any_cast<ftxui::Event>(data)};
        screen.PostEvent(screenEvent);
    } break;
    case tui::Event::InputEntered: {
        // TODO: handle input
        _bookList.getComponent()->TakeFocus();
    } break;

    case tui::Event::RefreshAll: {
        _storage.loadStorage();
        initComponents();
        _bottomLine.setMessage("Refreshed books and notes");
    } break;
    case tui::Event::RefreshBook: {
        if (_bookList.getSelectedID()) {
            updateNotesAndPreview(true);
            _bottomLine.setMessage(std::format("Refreshed book: {}", *_bookList.getSelectedID()));
        } else {
            _bottomLine.setMessage("No book to refresh");
        }
    } break;
    case tui::Event::RefreshNote: {
        if (auto bookID = _bookList.getSelectedID(); bookID) {
            if (_noteList.getSelectedID()) {
                redrawNotePreview();
                _bottomLine.setMessage(std::format("Refreshed note: {}", *_noteList.getSelectedID()));
            } else {
                _bottomLine.setMessage("No note to refresh");
            }
        }
    } break;

    case tui::Event::OpenEditor: {
        _bottomLine.setMessage(message);
    } break;

    default: {
        // TODO: add map with event names
        Log::warning("Unhandled event: {}", static_cast<int>(event));
    } break;
    }
}

}  // namespace linux
