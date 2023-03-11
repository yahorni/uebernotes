#pragma once

#include "core/config.hpp"
#include "core/storage.hpp"

namespace linux {

class Client {
public:
    explicit Client(const core::Config& cfg);
    core::Storage& getStorage();

    void listBooks();
    void createBook(std::string_view bookName);
    void createNote(core::BookID bookID, std::string_view noteName, std::string_view content);

private:
    core::Storage _storage;
};

}  // namespace linux
