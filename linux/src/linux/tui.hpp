#pragma once

#include "linux/tui/books.hpp"
#include "linux/tui/communicator.hpp"
#include "linux/tui/history.hpp"
#include "linux/tui/menu/index_cache.hpp"
#include "linux/tui/notes.hpp"
#include "linux/tui/preview.hpp"
#include "linux/tui/status.hpp"

#include <core/config.hpp>
#include <core/storage.hpp>

#include <ftxui/screen/screen.hpp>

#include <string>

namespace linux {

// TODO: separate:
// model load/updates with storage
// controller with handleCommand
// view with updateViews
// move TUI to tui namespace

class TUI {
public:
    explicit TUI(const core::Config& config);
    bool run();

private:
    core::Storage _storage;

    // TODO: move it out to restrict usage in handle*()
    tui::Communicator _communicator;

    tui::books::Controller _books;
    tui::notes::Controller _notes;
    tui::preview::Controller _preview;

    tui::history::Controller _history;
    tui::status::Controller _status;

    tui::menu::IndexCache<core::BookID> _noteIndexCache;

    void updateBooksModel(bool reload = false);
    void updateNotesModel(bool reload = false);
    void updatePreview();

    void resetFocus();

    using EventQueue = tui::Communicator::Queue<ftxui::Event>;
    void handleUIEvents(EventQueue& queue, ftxui::ScreenInteractive& screen);

    using CommandQueue = tui::Communicator::Queue<tui::Command>;
    void handleCommands(CommandQueue& queue);

    using NotificationQueue = tui::Communicator::Queue<std::string>;
    void handleNotifications(NotificationQueue& queue);
};

}  // namespace linux
