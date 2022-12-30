#pragma once

#include "core/types.hpp"

namespace linux {

class Client {
public:
    uebernotes::Storage& getStorage();

    void listBooks();
    void createBook(std::string_view book_name);

private:
    uebernotes::Storage _storage;
};

} // namespace linux
