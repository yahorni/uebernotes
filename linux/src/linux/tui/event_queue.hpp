#pragma once

#include <any>
#include <queue>
#include <string>
#include <tuple>

namespace linux::tui {

enum class Event {
    // Notifications
    BookChanged,
    NoteChanged,
    BookListUpdated,
    NoteListUpdated,

    // Commands
    /* set new message to status line */
    UpdateStatusLine,
    /* send UI event to screen to emulate screen event */
    PostScreenEvent,

    /* redraw books list, notes list and preview */
    RedrawBooks,
    /* redraw notes list and preview */
    RedrawNotes,
    /* redraw note preview */
    RedrawPreview,

    /* reload all books and notes */
    RefreshAll,
    /* reload notes for selected book */
    RefreshBook,
    /* reload selected note */
    RefreshNote,

    /* open external editor with selected note */
    OpenEditor,
};

class EventQueue {
public:
    using Value = std::tuple<Event, std::string, std::any>;
    std::queue<Value> _queue;

    // TODO: replace with std::variant
    void push(Event event, std::string&& message = "", std::any data = std::any{});
    Value pop();
    bool empty();
    std::size_t size();
};

}  // namespace linux::tui
