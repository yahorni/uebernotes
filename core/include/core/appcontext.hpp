#pragma once

#include <string>

namespace core {

struct AppContext {
    std::string database;
    bool useCaching;
};

}  // namespace core
