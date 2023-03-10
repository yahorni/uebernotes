#pragma once

#include "linux/argparser.hpp"

namespace linux {

class CLI {
public:
    explicit CLI(const CmdLineArgs& args);
};

}
