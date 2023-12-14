#include "linux/tui/event_queue.hpp"

#include <utility>

namespace linux::tui {

void EventQueue::push(Event event, std::string&& message, std::any data) {
    _queue.push(std::make_tuple(event, std::move(message), std::move(data)));
}

EventQueue::Value EventQueue::pop() {
    auto value = _queue.front();
    _queue.pop();
    return value;
}

bool EventQueue::empty() { return _queue.empty(); }

std::size_t EventQueue::size() { return _queue.size(); }

}  // namespace linux::tui
