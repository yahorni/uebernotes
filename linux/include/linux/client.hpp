#pragma once

#include "core/appcontext.hpp"
#include "core/storage.hpp"

namespace linux {

class Client {
public:
    explicit Client(const core::AppContext& context);
    core::Storage& getStorage();

    void listBooks();
    void printBook(core::BookID bookID);
    void printNote(core::NoteID noteID);
    void createBook(std::string_view bookName);
    void createNote(core::BookID bookID, std::string_view content);

private:
    core::Storage _storage;
};

}  // namespace linux
