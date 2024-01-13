#pragma once

#include <ftxui/component/event.hpp>

#include <format>
#include <queue>
#include <string>
#include <utility>

namespace linux::tui {

// TODO: separate to 3 categories:
// Notifications - just to show in status
//   + update Model for BottomLine
//   + set View for BottomLine to update
// Commands - change something through controllers
// UIEvent - pass UIEvent to screen object

enum class Command {
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
// TODO: test how Commands behave when mainthread is paused/sleeping
class Communicator {
public:
    template<typename Data>
    class Queue {
    public:
        void push(Data data) {
            if constexpr (std::is_integral_v<Data>) {
                _queue.push(data);
            } else {
                _queue.push(std::move(data));
            }
        }

        Data pop() {
            auto value = _queue.front();
            _queue.pop();
            return value;
        }

        bool size() const { return _queue.size(); }
        bool empty() const { return _queue.empty(); }

    private:
        std::queue<Data> _queue;
    };

    Queue<ftxui::Event> ui;
    Queue<Command> cmd;
    Queue<std::string> ntf;
};

}  // namespace linux::tui

template<>
struct std::formatter<linux::tui::Command> : std::formatter<const char*> {
    auto format(const linux::tui::Command& obj, std::format_context& ctx) const {
#define CMD_CASE(NAME)                                                                                                 \
    case linux::tui::Command::NAME:                                                                                    \
        return std::formatter<const char*>::format(#NAME, ctx);

        switch (obj) {
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
