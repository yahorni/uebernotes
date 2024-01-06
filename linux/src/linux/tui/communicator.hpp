#pragma once

#include <any>
#include <format>
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
    UpdateBookWhenOrderKept,
    UpdateBookWhenOrderChanged,
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

template<>
struct std::formatter<linux::tui::Command> : std::formatter<const char*> {
    auto format(const linux::tui::Command& obj, std::format_context& ctx) const {
#define CMD_CASE(NAME)                                                                                                 \
    case linux::tui::Command::NAME:                                                                                    \
        return std::formatter<const char*>::format(#NAME, ctx);

        switch (obj) {
            CMD_CASE(UIEvent);
            CMD_CASE(UpdateBookWhenOrderKept);
            CMD_CASE(UpdateBookWhenOrderChanged);
            CMD_CASE(UpdateNote);
            CMD_CASE(RefreshAll);
            CMD_CASE(RefreshBook);
            CMD_CASE(RefreshNote);
            CMD_CASE(InputEntered);
            CMD_CASE(InputCanceled);
            CMD_CASE(OpenEditor);
        default:
            return std::format_to(ctx.out(), "Unnamed<{}>", static_cast<int>(obj));
        }
#undef CMD_CASE
    }
};
