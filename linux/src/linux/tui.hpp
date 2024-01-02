#pragma once

#include "linux/tui/book_list.hpp"
#include "linux/tui/bottom_line.hpp"
#include "linux/tui/event_queue.hpp"
#include "linux/tui/note_list.hpp"
#include "linux/tui/preview_pane.hpp"
#include "linux/tui/history_panel.hpp"

#include <core/config.hpp>
#include <core/storage.hpp>

#include <ftxui/screen/screen.hpp>

#include <string>

namespace linux {

class TUI {
public:
    explicit TUI(const core::Config& config);
    bool run();

private:
    core::Storage _storage;

    tui::EventQueue _eventQueue;

    tui::book::View _bookView;
    tui::book::Model _bookModel;
    tui::book::Controller _bookController;

    tui::NoteList _noteList;
    tui::PreviewPane _previewPane;
    tui::HistoryPanel _historyPanel;
    tui::BottomLine _bottomLine;

    void initComponents();
    void updateNotesAndPreview(bool useCached = true);
    void redrawNotePreview();
    void resetFocus();

    void handleCommands(ftxui::ScreenInteractive& screen);
    void handleMessage(const std::string& message);
};

}  // namespace linux
