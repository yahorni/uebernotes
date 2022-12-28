#include "note.hpp"

namespace uebernotes {

// NoteInfo
NoteInfo::NoteInfo(std::string name, std::string content)
    : NoteInfo(0, 0, std::move(name), std::move(content)) {}

NoteInfo::NoteInfo(NoteID id, BookID bookID, std::string&& name, std::string&& content)
    : id(id),
      bookID(bookID),
      name(std::move(name)),
      content(std::move(content)) {}

// Note
Note::Note(NoteInfo note, Database& db)
    : _note(std::move(note)),
      _db(db) {}

void Note::updateContent(std::string&&) { printf("TODO: implement Note::updateContent()\n"); }

} // namespace uebernotes
