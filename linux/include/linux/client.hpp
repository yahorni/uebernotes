#pragma once

#include "core/appcontext.hpp"
#include "core/storage.hpp"

#include <string>

namespace linux {

class Client {
public:
    explicit Client(const core::AppContext& context);

    bool listBooks();

    bool printBook(core::BookID bookID);
    bool createBook(std::string&& name);
    bool updateBook(core::BookID bookID, std::string&& name);
    bool removeBook(core::BookID bookID);

    bool printNote(core::NoteID noteID);
    bool createNote(core::BookID bookID, std::string&& content);
    bool updateNote(core::NoteID noteID, std::string&& content);
    bool removeNote(core::NoteID noteID);

private:
    core::Storage _storage;
};

}  // namespace linux
