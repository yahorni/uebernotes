#include "core/note.hpp"

#include "core/storage.hpp"

namespace core {

// NoteInfo
NoteInfo::NoteInfo(BookID bookID, std::string&& content)
    : NoteInfo(0, bookID, std::move(content)) {}

NoteInfo::NoteInfo(NoteID id, BookID bookID, std::string&& content)
    : id(id),
      bookID(bookID),
      content(std::move(content)) {}

NoteInfo::NoteInfo(NoteInfo&& other)
    : NoteInfo(other.id, other.bookID, std::move(other.content)) {}

// Note
Note::Note(std::shared_ptr<NoteInfo> note)
    : _note(std::move(note)) {}

BookID Note::getBookID() const { return _note->bookID; }

const std::string& Note::getContent() const { return _note->content; }

}  // namespace core
