#pragma once

#include "linux/argparser.hpp"

#include <core/config.hpp>
#include <core/storage.hpp>

#include <string>

namespace linux {

class CLI {
public:
    explicit CLI(const core::Config& config);

    bool run(const CmdLineArgs& args);

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
