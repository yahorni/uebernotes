#pragma once

#include "linux/argparser.hpp"

namespace linux {

class TUI {
public:
    explicit TUI(const CmdLineArgs& args);

    bool run();
};

}  // namespace linux
