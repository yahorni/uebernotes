#pragma once

#include "linux/tui/books.hpp"
#include "linux/tui/bottom_line.hpp"
#include "linux/tui/communicator.hpp"
#include "linux/tui/history_panel.hpp"
#include "linux/tui/menu/index_cache.hpp"
#include "linux/tui/notes.hpp"
#include "linux/tui/preview.hpp"

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

    tui::books::Controller _bookController;
    tui::notes::Controller _noteController;
    tui::preview::Controller _previewController;

    tui::HistoryPanel _historyPanel;
    tui::BottomLine _bottomLine;

    tui::menu::IndexCache<core::BookID> _noteIndexCache;

    void updateBooksModel(bool reload = false);
    void updateNotesModel(bool reload = false);
    void updatePreview();

    void resetFocus();

    void handleCommands(ftxui::ScreenInteractive& screen);
    void handleNotifications();
};

}  // namespace linux
