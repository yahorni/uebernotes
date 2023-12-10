#pragma once

#include "core/comparator.hpp"
#include "core/types.hpp"

#include <memory>
#include <string>
#include <unordered_set>

namespace core {

class Storage;

struct NoteInfo {
    NoteID id;
    BookID bookID;
    std::string content;

    explicit NoteInfo(BookID bookID = 0, std::string&& content = "");
    NoteInfo(NoteID id, BookID bookID, std::string&& content);
    NoteInfo(const NoteInfo&) = delete;
    NoteInfo(NoteInfo&&);

    BookID getBookID() const;
    std::string_view getName() const;
    const std::string& getContent() const;
};

using NotePtr = std::shared_ptr<NoteInfo>;

// TODO: later add sorting by creation time
using NotesCache = std::unordered_set<NotePtr, SharedPtrExtension::HashID, SharedPtrExtension::CompareID>;

}  // namespace core
