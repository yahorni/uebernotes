#pragma once

#include "core/config.hpp"
#include "linux/argparser.hpp"

#include <string_view>

namespace linux {

class TUI {
public:
    explicit TUI(const core::Config& config);
    bool run();
};

}  // namespace linux
