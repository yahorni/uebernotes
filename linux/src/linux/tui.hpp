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

namespace linux {

// TODO: separate:
// model load/updates with storage
// controller with handleCommand
// view with updateViews

class TUI {
public:
    explicit TUI(const core::Config& config);
    bool run();

private:
    core::Storage _storage;

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

    void handleCommands(ftxui::ScreenInteractive& screen);
    void handleNotifications();
};

}  // namespace linux
