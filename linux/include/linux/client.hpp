#pragma once

#include "core/types.hpp"

namespace linux {

class Client {
public:
    uebernotes::Storage& getStorage();

private:
    uebernotes::Storage _storage;
};

} // namespace linux
