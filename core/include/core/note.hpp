#pragma once

#include "core/types.hpp"

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace core {

class Storage;

struct NoteInfo {
    NoteID id;
    BookID bookID;
    std::string content;

    explicit NoteInfo(std::string content = "");             // for manual creation
    NoteInfo(NoteID id, BookID bookID, std::string&& content);  // for database
    NoteInfo(const NoteInfo&) = delete;
    NoteInfo(NoteInfo&&);
};

class Note {
public:
    Note(NoteInfo note, Storage& storage);

    const std::string& getContent() const;
    void updateContent(std::string&& newContent);

private:
    NoteInfo _note;
    Storage& _storage;
};

// TODO: add sorting by ID (later: by creation time)
using NotesInfoCollection = std::list<std::shared_ptr<NoteInfo>>;

}  // namespace core
