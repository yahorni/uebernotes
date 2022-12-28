#pragma once

#include <cstdint>
#include <list>
#include <string>

#include "common_types.hpp"

namespace uebernotes {

struct NoteInfo {
    NoteID id;
    BookID bookID;
    std::string name;
    std::string content;

    explicit NoteInfo(std::string name = "", std::string content = "");            // for manual creation
    NoteInfo(NoteID id, BookID bookID, std::string&& name, std::string&& content); // for database
};

using NotesInfoCollection = std::list<NoteInfo>;

class Note {
public:
    Note(NoteInfo note, Database& db);

    void updateContent(std::string&& newContent);

private:
    NoteInfo _note;
    Database& _db;
};

} // namespace uebernotes
