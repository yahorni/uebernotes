#pragma once

#include "linux/tui/book_list.hpp"
#include "linux/tui/bottom_line.hpp"
#include "linux/tui/event_queue.hpp"
#include "linux/tui/history_panel.hpp"
#include "linux/tui/note_list.hpp"
#include "linux/tui/preview_pane.hpp"

#include <core/config.hpp>
#include <core/storage.hpp>

#include <ftxui/screen/screen.hpp>

#include <string>

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

    tui::EventQueue _eventQueue;

    tui::book::Model _bookModel;
    tui::book::View _bookView;
    tui::book::Controller _bookController;

    tui::note::Model _noteModel;
    tui::note::View _noteView;
    tui::note::Controller _noteController;

    tui::PreviewPane _previewPane;
    tui::HistoryPanel _historyPanel;
    tui::BottomLine _bottomLine;

    void initComponents();
    void updateNotesAndPreview(bool useCached = true);
    void redrawNotePreview();
    void resetFocus();

    void handleCommand(ftxui::ScreenInteractive& screen);
    void handleMessage(const std::string& message);
};

}  // namespace linux
