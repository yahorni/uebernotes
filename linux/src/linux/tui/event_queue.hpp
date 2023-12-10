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
};

class EventQueue {
public:
    std::queue<std::tuple<Event, std::any>> _queue;

    void push(Event event, std::any data = std::any{}) {  // replace with std::variant
        _queue.push(std::make_tuple(event, data));
    }

    bool empty() { return _queue.empty(); }

    std::tuple<Event, std::any> pop() {
        auto cmd = _queue.front();
        _queue.pop();
        return cmd;
    }
};

}  // namespace linux::tui
