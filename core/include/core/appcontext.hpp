#pragma once

#include <string>

namespace core {

struct AppContext {
    std::string database;
    bool hasInitialOperation;
};

}  // namespace core
