#pragma once

#include "core/appcontext.hpp"
#include "linux/argparser.hpp"
#include "linux/client.hpp"

#include <string_view>

namespace linux {

class TUI {
public:
    explicit TUI(const core::AppContext& context);
    bool run();

private:
    Client _client;
};

}  // namespace linux
