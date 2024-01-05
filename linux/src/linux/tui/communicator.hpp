#pragma once

#include <any>
#include <queue>
#include <string>
#include <tuple>

namespace linux::tui {

// TODO: separate to 3 categories:
// Notifications - just to show in status
//   + update Model for BottomLine
//   + set View for BottomLine to update
// Commands - change something through controllers
// UIEvent - pass UIEvent to screen object

enum class Command {
    UIEvent,
    UpdateBook,
    UpdateNote,
    RefreshAll,
    RefreshBook,
    RefreshNote,
    InputEntered,
    InputCanceled,
    OpenEditor,
};

// TODO: rename it
class Communicator {
public:
    // Commands
    using CommandData = std::tuple<Command, std::any>;
    std::queue<CommandData> _commands;
    //
    // TODO: replace with std::variant
    void cmdPush(Command cmd, std::any data = std::any{});
    CommandData cmdPop();
    bool cmdEmpty();
    std::size_t cmdSize();

    // Notifications
    std::queue<std::string> _notifications;
    //
    void ntfPush(std::string&& message);
    std::string ntfPop();
    bool ntfEmpty();
    std::size_t ntfSize();
};

}  // namespace linux::tui
