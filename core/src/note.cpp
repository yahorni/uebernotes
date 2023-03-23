#include "core/note.hpp"

#include "core/storage.hpp"

namespace core {

// NoteInfo
NoteInfo::NoteInfo(std::string name, std::string content)
    : NoteInfo(0, 0, std::move(name), std::move(content)) {}

NoteInfo::NoteInfo(NoteID id, BookID bookID, std::string&& name, std::string&& content)
    : id(id),
      bookID(bookID),
      name(std::move(name)),
      content(std::move(content)) {}

NoteInfo::NoteInfo(NoteInfo&& other)
    : NoteInfo(other.id, other.bookID, std::move(other.name), std::move(other.content)) {}

// Note
Note::Note(NoteInfo note, Storage& storage)
    : _note(std::move(note)),
      _storage(storage) {}

const std::string& Note::getName() const { return _note.name; }
const std::string& Note::getContent() const { return _note.content; }

// TODO: update data in db
void Note::updateName(std::string&& newName) { _note.name = std::move(newName); }
void Note::updateContent(std::string&& newContent) { _note.content = std::move(newContent); }

}  // namespace core
