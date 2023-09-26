#pragma once

#include "core/appcontext.hpp"
#include "core/storage.hpp"
#include "linux/argparser.hpp"

#include <string_view>

namespace linux {

class TUI {
public:
    explicit TUI(const core::AppContext& context);
    bool run();

private:
    core::Storage _storage;
};

}  // namespace linux
