#pragma once

#include "core/config.hpp"
#include "linux/argparser.hpp"
#include "linux/client.hpp"

#include <string_view>

namespace linux {

class CLI {
public:
    explicit CLI(const core::Config& config);
    void run(const CmdLineArgs& args);

private:
    Client _client;
};

}  // namespace linux
