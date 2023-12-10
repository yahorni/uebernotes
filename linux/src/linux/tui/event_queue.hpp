#pragma once

#include <any>
#include <queue>
#include <tuple>

namespace linux::tui {

enum class Event {
    // Notifications
    BookChanged,
    NoteChanged,

    // Commands
    /* sets new message to status line */
    UpdateStatusLine,
    /* sends UI event to screen to emulate screen event */
    PostScreenEvent,
    /* reloads all books and notes */
    RefreshAll,
    /* reloads notes for selected book */
    RefreshBook,
    /* reloads selected note */
    RefreshNote,
    /* opens external editor with selected note */
    OpenEditor,
    /* toggle showing of book IDs in list*/
    ToggleShowBookID,
    /* toggle showing of note IDs in list*/
    ToggleShowNoteID,
};

class EventQueue {
public:
    std::queue<std::tuple<Event, std::any>> _queue;

    // TODO: replace with std::variant
    void push(Event event, std::any data = std::any{});
    bool empty();
    std::tuple<Event, std::any> pop();
};

}  // namespace linux::tui
