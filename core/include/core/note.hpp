#pragma once

#include <cstdint>
#include <list>
#include <string>

#include "core/common_types.hpp"

namespace core {

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

    const std::string& getName() const;
    const std::string& getContent() const;

    void updateName(std::string&& newName);
    void updateContent(std::string&& newContent);

private:
    NoteInfo _note;
    Database& _db;
};

} // namespace core
