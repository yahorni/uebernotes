#pragma once

#include <string>

namespace core {

struct Config {
    std::string database;
    bool useCaching;
};

}  // namespace core
