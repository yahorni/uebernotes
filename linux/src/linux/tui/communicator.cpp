#include "linux/tui/communicator.hpp"

#include <utility>

namespace linux::tui {

// commands

void Communicator::cmdPush(Command cmd, std::any&& data) {
    _commands.push(std::make_tuple(cmd, std::move(data)));
}

Communicator::CommandData Communicator::cmdPop() {
    auto value = _commands.front();
    _commands.pop();
    return value;
}

bool Communicator::cmdEmpty() { return _commands.empty(); }

std::size_t Communicator::cmdSize() { return _commands.size(); }

// notifications

void Communicator::ntfPush(std::string&& message) {
    _notifications.push(std::move(message));
}

std::string Communicator::ntfPop() {
    auto value = _notifications.front();
    _notifications.pop();
    return value;
}

bool Communicator::ntfEmpty() { return _notifications.empty(); }

std::size_t Communicator::ntfSize() { return _notifications.size(); }

}  // namespace linux::tui
