#include "linux/tui.hpp"

#include "linux/logger.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/string.hpp>

#include <string>
#include <utility>

// ftxui/dom/elements.hpp - list of elements to create layouts
// ftxui/dom/component.hpp - interactive components to respond to events

// TODO: properly handle segfault

namespace linux {

TUI::TUI(const core::Config& config)
    : _storage{config},
      _noteIndexCache(_notes.createIndexCache<core::BookID>()) {
    // create components
    _books.createComponent(_communicator);
    _notes.createComponent(_communicator);
    _preview.createComponent(_communicator);
    _history.createComponent(_communicator);
    _status.createComponent(_communicator);

    updateBooksModel();
    updateNotesModel();
    updatePreview();
}

bool TUI::run() {
    using namespace ftxui;  // NOLINT

    auto screen{ScreenInteractive::Fullscreen()};
    ftxui::Dimensions minWinSize{80, 20};

    auto commonEvents = CatchEvent([&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('R')) {
            _communicator.cmd.push(tui::Command::RefreshAll);
            return true;
        } else if (event == Event::Character('/')) {
            _status.setMode(tui::status::Mode::Search);
            _status.component()->TakeFocus();
            return true;
        } else if (event == Event::Character(':')) {
            _status.setMode(tui::status::Mode::Command);
            _status.component()->TakeFocus();
            return true;
        } else if (event == Event::Character('t')) {
            // toggle history view
            //
            if (!_history.toggleView()) {
                // TODO: autoreset focus without manual
                resetFocus();
            }
            return true;
        } else if (event == Event::Character('T')) {
            // focus on history view if enabled
            //
            auto& historyComponent = *_history.component();
            if (historyComponent.Focused()) {
                resetFocus();
            } else if (historyComponent.Focusable()) {
                historyComponent.TakeFocus();
            }
            return true;
        }
        return false;
    });

    auto dataPanes = Container::Horizontal({
        _books.component(),
        _notes.component(),
        _preview.component(),
    }) | ftxui::EventHandler({ftxui::Event::Character('j')});

    auto panesContainer = Container::Vertical({dataPanes, _history.component()}) | commonEvents;
    auto container = Container::Vertical({panesContainer, _status.component()});

    auto renderer = Renderer(container, [&] {
        Log::debug("Render, cmds={}, ntfs={}", _communicator.cmd.size(), _communicator.ntf.size());

        auto winSize = Terminal::Size();
        if (winSize.dimx < minWinSize.dimx || winSize.dimy < minWinSize.dimy) {
            return paragraph(std::format("too small window: {}x{}", winSize.dimx, winSize.dimy));
        }

        handleUIEvents(_communicator.ui, screen);
        handleCommands(_communicator.cmd);
        handleNotifications(_communicator.ntf);

        int listWidth = winSize.dimx / 4;
        int historyHeight = winSize.dimy / 4;
        return vbox({
            hbox({
                _books.element(listWidth),  //
                _notes.element(listWidth),  //
                _preview.element(),         //
            }) | yflex,
            _history.element(historyHeight),
            _status.element(),
        });
    });

    screen.Loop(renderer);
    return true;
}

void TUI::updateBooksModel(bool reload) {
    if (reload) {
        _storage.loadBooksToCache();
    }
    _books.setItems(_storage.getBooks());
}

void TUI::updateNotesModel(bool reload) {
    if (const auto& bookID = _books.getSelectedItemID(); bookID) {
        if (reload) {
            _storage.loadNotesToCache(*bookID);
        }
        _notes.setItems(_storage.getNotesByBookID(*bookID));
    }
}

void TUI::updatePreview() {
    auto notePtr = _notes.getSelectedItem();
    _preview.setNote(notePtr);
}

void TUI::resetFocus() { _books.component()->TakeFocus(); }

void TUI::handleUIEvents(TUI::EventQueue& queue, ftxui::ScreenInteractive& screen) {
    // TODO: test behaviour
    while (!queue.empty()) {
        auto event = queue.pop();
        screen.PostEvent(event);
    }
}

void TUI::handleCommands(TUI::CommandQueue& queue) {
    while (!queue.empty()) {
        auto command = queue.pop();
        Log::debug("Handling command: {}", command);

        switch (command) {
        case tui::Command::UpdateBookWhenOrderKept: {
            // notes
            updateNotesModel();
            _noteIndexCache.restore(_books.getSelectedIndex());

            // preview
            updatePreview();
        } break;
        case tui::Command::UpdateBookWhenOrderChanged: {
            // notes
            updateNotesModel();
            _noteIndexCache.clear();

            // preview
            updatePreview();
        } break;
        case tui::Command::UpdateNote: {
            // notes
            _noteIndexCache.add(_books.getSelectedIndex());

            // preview
            updatePreview();
        } break;

        case tui::Command::RefreshAll: {
            // books
            updateBooksModel(true);

            // notes
            updateNotesModel();
            _noteIndexCache.clear();

            // preview
            updatePreview();
        } break;
        case tui::Command::RefreshBook: {
            // notes
            updateNotesModel(true);
            _noteIndexCache.remove(_books.getSelectedIndex());

            // preview
            updatePreview();
        } break;
        case tui::Command::RefreshNote: {
            // preview
            updatePreview();
        } break;

        case tui::Command::InputEntered: {
            // TODO: handle input
            auto input = _status.getLastInput();
            _communicator.ntf.push("TODO: Handle status input");

            // main
            resetFocus();
        } break;
        case tui::Command::InputCanceled: {
            // main
            resetFocus();
        } break;

        case tui::Command::OpenEditor: {
            if (_notes.getItemsAmount()) {
                _communicator.ntf.push("TODO: Open editor");
            } else {
                _communicator.ntf.push("No selected note to open");
            }
        } break;

        default: {
            // TODO: add map with command names
            Log::warning("Unhandled command: {}", static_cast<int>(command));
        } break;
        }
    }
}

void TUI::handleNotifications(TUI::NotificationQueue& queue) {
    while (!queue.empty()) {
        auto notification = queue.pop();
        Log::info("Handling notification: '{}'", notification);

        if (notification.empty()) {
            return;
        }

        _status.setMessage(notification);
        _history.addMessage(std::move(notification));
    }
}

}  // namespace linux
