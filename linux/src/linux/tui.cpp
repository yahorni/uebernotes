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
      _bottomLine{_communicator},
      _noteIndexCache(_noteController.createIndexCache<core::BookID>()) {
    // create components
    _bookController.createComponent(_communicator);
    _noteController.createComponent(_communicator);
    _previewController.createComponent(_communicator);

    updateBooksModel();
    updateNotesModel();
    updatePreview();
}

bool TUI::run() {
    using namespace ftxui;  // NOLINT

    auto screen{ScreenInteractive::Fullscreen()};
    ftxui::Dimensions minWinSize{80, 20};

    auto panesContainer = Container::Vertical({Container::Horizontal({
                                                   _bookController.component(),
                                                   _noteController.component(),
                                                   _previewController.component(),
                                               }),
                                               _historyPanel.getComponent()});

    auto panesContainerWithEvents = CatchEvent(panesContainer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('R')) {
            _communicator.cmdPush(tui::Command::RefreshAll);
            return true;
        } else if (event == Event::Character('/')) {
            _bottomLine.setMode(tui::BottomLine::Mode::Search);
            _bottomLine.getComponent()->TakeFocus();
            return true;
        } else if (event == Event::Character(':')) {
            _bottomLine.setMode(tui::BottomLine::Mode::Command);
            _bottomLine.getComponent()->TakeFocus();
            return true;
        } else if (event == Event::Character('t')) {
            if (!_historyPanel.toggle()) {
                // TODO: autoreset focus without manual
                resetFocus();
            }
            return true;
        } else if (event == Event::Character('T')) {
            auto& historyComponent = *_historyPanel.getComponent();
            if (historyComponent.Focused()) {
                resetFocus();
            } else if (historyComponent.Focusable()) {
                historyComponent.TakeFocus();
            }
            return true;
        }
        return false;
    });

    auto container = Container::Vertical({panesContainerWithEvents, _bottomLine.getComponent()});

    auto renderer = Renderer(container, [&] {
        Log::debug("render, cmds={}, ntfs={}", _communicator.cmdSize(), _communicator.ntfSize());

        auto winSize = Terminal::Size();
        if (winSize.dimx < minWinSize.dimx || winSize.dimy < minWinSize.dimy) {
            return paragraph(std::format("too small window: {}x{}", winSize.dimx, winSize.dimy));
        }

        handleCommands(screen);
        handleNotifications();

        int listPaneSize = winSize.dimx / 4;
        int historyPanelSize = winSize.dimy / 4;
        return vbox({
            hbox({
                _bookController.element(listPaneSize),  //
                _noteController.element(listPaneSize),  //
                _previewController.element(),           //
            }) | yflex,
            _historyPanel.getElement(historyPanelSize),
            _bottomLine.getElement(),
        });
    });

    screen.Loop(renderer);
    return true;
}

void TUI::updateBooksModel(bool reload) {
    if (reload) {
        _storage.loadBooksToCache();
    }
    _bookController.setItems(_storage.getBooks());
}

void TUI::updateNotesModel(bool reload) {
    if (const auto& bookID = _bookController.getSelectedItemID(); bookID) {
        if (reload) {
            _storage.loadNotesToCache(*bookID);
        }
        _noteController.setItems(_storage.getNotesByBookID(*bookID));
    }
}

void TUI::updatePreview() {
    auto notePtr = _noteController.getSelectedItem();
    _previewController.setNote(notePtr);
}

void TUI::resetFocus() { _bookController.component()->TakeFocus(); }

void TUI::handleCommands(ftxui::ScreenInteractive& screen) {
    // FIXME: restore note index during scrolling in book menu
    while (!_communicator.cmdEmpty()) {
        auto [command, data] = _communicator.cmdPop();
        Log::debug("handling command: {}", command);

        switch (command) {
        case tui::Command::UIEvent: {
            auto screenEvent{std::any_cast<ftxui::Event>(data)};
            screen.PostEvent(screenEvent);
        } break;

        case tui::Command::UpdateBookWhenOrderKept: {
            // notes
            updateNotesModel();
            _noteIndexCache.restore(_bookController.getSelectedIndex());

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
            _noteIndexCache.add(_bookController.getSelectedIndex());

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
            _noteIndexCache.remove(_bookController.getSelectedIndex());

            // preview
            updatePreview();
        } break;
        case tui::Command::RefreshNote: {
            // preview
            updatePreview();
        } break;

        case tui::Command::InputEntered: {
            // TODO: handle input
            auto input = _bottomLine.getLastInput();

            // main
            resetFocus();
        } break;
        case tui::Command::InputCanceled: {
            // main
            resetFocus();
        } break;

        case tui::Command::OpenEditor: {
            // TODO
        } break;

        default: {
            // TODO: add map with command names
            Log::warning("Unhandled command: {}", static_cast<int>(command));
        } break;
        }
    }
}

void TUI::handleNotifications() {
    while (!_communicator.ntfEmpty()) {
        auto message = _communicator.ntfPop();

        if (!message.empty()) {
            return;
        }

        Log::info("Notification: {}", message);
        _bottomLine.setMessage(message);
        _historyPanel.addMessage(std::move(message));
    }
}

}  // namespace linux
