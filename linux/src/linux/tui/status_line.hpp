#pragma once

#include "linux/tui/context.hpp"

#include <ftxui/component/component.hpp>

namespace linux::tui {

class StatusLine {
public:
    explicit StatusLine(Context* ctx)
        : _ctx(ctx) {}

    ftxui::Element getElement() const { return ftxui::text(_ctx->lastMessage.c_str()); }

private:
    Context* _ctx{nullptr};
};

}  // namespace linux::tui
