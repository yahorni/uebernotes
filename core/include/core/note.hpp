#pragma once

#include "core/comparator.hpp"
#include "core/types.hpp"

#include <cstdint>
#include <list>
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
};

class Note {
public:
    explicit Note(std::shared_ptr<NoteInfo> note);

    BookID getBookID() const;
    const std::string& getContent() const;

private:
    std::shared_ptr<NoteInfo> _note;
};

// TODO: later add sorting by creation time
using NotesCache = std::unordered_set<std::shared_ptr<NoteInfo>, SharedPtrWithID::Hash, SharedPtrWithID::Compare>;

}  // namespace core
