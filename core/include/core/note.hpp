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

    explicit NoteInfo(std::string content = "");                // for manual creation
    NoteInfo(NoteID id, BookID bookID, std::string&& content);  // for database
    NoteInfo(const NoteInfo&) = delete;
    NoteInfo(NoteInfo&&);
};

class Note {
public:
    Note(std::shared_ptr<NoteInfo> note);

    const std::string& getContent() const;
    void updateContent(std::string&& newContent);

private:
    std::shared_ptr<NoteInfo> _note;
};

// TODO: later add sorting by creation time
using NotesCache = std::unordered_set<std::shared_ptr<NoteInfo>, SharedPtrWithID::Hash, SharedPtrWithID::Compare>;

}  // namespace core
