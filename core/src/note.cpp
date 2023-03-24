#include "core/note.hpp"

#include "core/storage.hpp"

namespace core {

// NoteInfo
NoteInfo::NoteInfo(std::string content)
    : NoteInfo(0, 0, std::move(content)) {}

NoteInfo::NoteInfo(NoteID id, BookID bookID, std::string&& content)
    : id(id),
      bookID(bookID),
      content(std::move(content)) {}

NoteInfo::NoteInfo(NoteInfo&& other)
    : NoteInfo(other.id, other.bookID, std::move(other.content)) {}

// Note
Note::Note(NoteInfo note, Storage& storage)
    : _note(std::move(note)),
      _storage(storage) {}

const std::string& Note::getContent() const { return _note.content; }

// TODO: update data in db
void Note::updateContent(std::string&& newContent) { _note.content = std::move(newContent); }

}  // namespace core
