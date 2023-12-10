#pragma once

#include "linux/tui/book_list.hpp"
#include "linux/tui/event_queue.hpp"
#include "linux/tui/note_list.hpp"
#include "linux/tui/preview_pane.hpp"
#include "linux/tui/status_line.hpp"

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
    tui::StatusLine _statusLine;

    void updateComponents();
    bool updateNoteComponents(bool refresh = false);
    bool updateNotePreview(core::BookID bookID);

    void handleCommands(ftxui::ScreenInteractive& screen);
};

}  // namespace linux
