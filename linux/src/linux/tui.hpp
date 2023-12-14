#pragma once

#include "linux/tui/book_list.hpp"
#include "linux/tui/bottom_line.hpp"
#include "linux/tui/event_queue.hpp"
#include "linux/tui/note_list.hpp"
#include "linux/tui/preview_pane.hpp"

#include <core/config.hpp>
#include <core/storage.hpp>

#include <ftxui/screen/screen.hpp>

namespace linux {

class TUI {
public:
    explicit TUI(const core::Config& config);
    bool run();

private:
    core::Storage _storage;

    tui::EventQueue _eventQueue;

    tui::BookList _bookList;
    tui::NoteList _noteList;
    tui::PreviewPane _previewPane;
    tui::BottomLine _bottomLine;

    void initComponents();
    void updateNotesAndPreview(bool useCached = true);
    void redrawNotePreview();

    void handleCommands(ftxui::ScreenInteractive& screen);
};

}  // namespace linux
