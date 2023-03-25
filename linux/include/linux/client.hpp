#pragma once

#include "core/appcontext.hpp"
#include "core/storage.hpp"

#include <string>

namespace linux {

class Client {
public:
    explicit Client(const core::AppContext& context);

    void listBooks();

    void printBook(core::BookID bookID);
    void createBook(std::string&& name);
    void updateBook(core::BookID bookID, std::string&& name);
    void removeBook(core::BookID bookID);

    void printNote(core::NoteID noteID);
    void createNote(core::BookID bookID, std::string&& content);
    void updateNote(core::NoteID noteID, std::string&& content);
    void removeNote(core::NoteID noteID);

private:
    core::Storage _storage;
};

}  // namespace linux
