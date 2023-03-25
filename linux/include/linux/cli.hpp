#pragma once

#include "core/appcontext.hpp"
#include "linux/argparser.hpp"
#include "linux/client.hpp"

#include <string_view>

namespace linux {

class CLI {
public:
    explicit CLI(const core::AppContext& config);
    bool run(const CmdLineArgs& args);

private:
    Client _client;
};

}  // namespace linux
