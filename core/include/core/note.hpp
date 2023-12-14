#pragma once

#include "core/comparator.hpp"
#include "core/types.hpp"

#include <memory>
#include <string>
#include <unordered_set>

namespace core {

class Storage;

struct Note {
    NoteID id;
    BookID bookID;
    std::string content;

    explicit Note(BookID bookID = 0, std::string&& content = "");
    Note(NoteID id, BookID bookID, std::string&& content);
    Note(const Note&) = delete;
    Note(Note&&);

    BookID getBookID() const;
    std::string_view getName() const;
    const std::string& getContent() const;
};

using NotePtr = std::shared_ptr<Note>;

// TODO: later add sorting by creation time
using NotesCache = std::unordered_set<NotePtr, SharedPtrExtension::HashID, SharedPtrExtension::CompareID<>>;

}  // namespace core
