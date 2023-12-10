#include "linux/tui/event_queue.hpp"

namespace linux::tui {

void EventQueue::push(Event event, std::any data) { _queue.push(std::make_tuple(event, data)); }

bool EventQueue::empty() { return _queue.empty(); }

std::tuple<Event, std::any> EventQueue::pop() {
    auto cmd = _queue.front();
    _queue.pop();
    return cmd;
}

}  // namespace linux::tui
