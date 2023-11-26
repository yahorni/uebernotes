#pragma once

#include "core/config.hpp"
#include "core/storage.hpp"

namespace linux {

class TUI {
public:
    explicit TUI(const core::Config& config);
    bool run();

private:
    core::Storage _storage;
};

}  // namespace linux
