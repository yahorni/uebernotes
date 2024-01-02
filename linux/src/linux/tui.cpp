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
      _bookController{&_eventQueue, _bookModel, _bookView},
      _noteList{&_storage, &_eventQueue},
      _previewPane{&_eventQueue},
      _bottomLine(&_eventQueue) {
    _bookController.createComponent();
}

bool TUI::run() {
    using namespace ftxui;  // NOLINT

    auto screen{ScreenInteractive::Fullscreen()};
    ftxui::Dimensions minWinSize{80, 20};

    auto panesContainer = Container::Vertical({Container::Horizontal({
                                                   _bookModel.getComponent(),
                                                   _noteList.getComponent(),
                                                   _previewPane.getComponent(),
                                               }),
                                               _historyPanel.getComponent()});

    auto panesContainerWithEvents = CatchEvent(panesContainer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        } else if (event == Event::Character('R')) {
            _eventQueue.push(tui::Event::RefreshAll, "Refreshed books and notes");
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

    initComponents();

    auto renderer = Renderer(container, [&] {
        Log::debug("render, events: {}", _eventQueue.size());

        auto winSize = Terminal::Size();
        if (winSize.dimx < minWinSize.dimx || winSize.dimy < minWinSize.dimy) {
            return paragraph(std::format("too small window: {}x{}", winSize.dimx, winSize.dimy));
        }

        handleCommands(screen);

        int listPaneSize = winSize.dimx / 4;
        int historyPanelSize = winSize.dimy / 4;
        return vbox({
            hbox({
                _bookView.getElement(_bookModel.getComponent(), listPaneSize),  //
                _noteList.getElement(listPaneSize),                             //
                _previewPane.getElement(),                                      //
            }) | yflex,
            _historyPanel.getElement(historyPanelSize),
            _bottomLine.getElement(),
        });
    });

    screen.Loop(renderer);
    return true;
}

void TUI::initComponents() {
    _bookView.resetIndex();
    _noteList.reset();
    _previewPane.reset();

    _bookController.setItems(_storage.getBooks());
    updateNotesAndPreview();
}

void TUI::updateNotesAndPreview(bool useCached) {
    auto bookID = _bookController.getSelectedItemID();
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

void TUI::resetFocus() { _bookModel.getComponent()->TakeFocus(); }

void TUI::handleMessage(const std::string& message) {
    // TODO: resolve problem with setting same string in _bottomLine after InputEntered
    _bottomLine.setMessage(message);
    _historyPanel.addMessage(std::string(message));
    Log::info("{}", message);
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
        if (auto bookID = _bookController.getSelectedItemID(); bookID) {
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
        handleMessage(message);
    } break;
    case tui::Event::PostScreenEvent: {
        auto screenEvent{std::any_cast<ftxui::Event>(data)};
        screen.PostEvent(screenEvent);
    } break;
    case tui::Event::InputEntered: {
        // TODO: handle input
        auto input{std::any_cast<std::string>(data)};
        _historyPanel.addMessage(std::move(message));
        resetFocus();
    } break;
    case tui::Event::InputCanceled: {
        resetFocus();
    } break;

    case tui::Event::RefreshAll: {
        handleMessage(message);
        _storage.loadStorage();
        initComponents();
    } break;
    case tui::Event::RefreshBook: {
        handleMessage(message);
        updateNotesAndPreview(true);
    } break;
    case tui::Event::RefreshNote: {
        handleMessage(message);
        redrawNotePreview();
    } break;

    case tui::Event::OpenEditor: {
        handleMessage(message);
    } break;

    default: {
        // TODO: add map with event names
        Log::warning("Unhandled event: {}", static_cast<int>(event));
    } break;
    }
}

}  // namespace linux
