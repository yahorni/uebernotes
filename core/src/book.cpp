#include "core/book.hpp"

#include "core/database.hpp"

namespace core {

// BookInfo
BookInfo::BookInfo(std::string&& name)
    : BookInfo(0, std::move(name)) {}

BookInfo::BookInfo(BookID id, std::string&& name)
    : id(id),
      name(std::move(name)) {}

// Book
Book::Book(BookInfo book, Database& db)
    : _book(std::move(book)),
      _db(db) {}

const BookInfo& Book::getBookInfo() const { return _book; }
const NotesInfoCollection& Book::getNotesInfo() const { return _notes; }

void Book::loadNotes() { _notes = _db.loadNotesByBookID(_book.id); }

std::optional<NoteID> Book::createNote(NoteInfo&& note) {
    note.bookID = _book.id;
    NoteID id = _db.storeNote(note);                              // 1st DB call
    std::optional<NoteInfo> insertedNote = _db.loadNoteByID(id);  // 2nd DB call
    if (insertedNote.has_value()) {
        _notes.emplace_back(std::move(insertedNote.value()));
        return id;
    }
    return std::nullopt;
}

std::optional<Note> Book::getNote(NoteID noteID) const {
    std::optional<NoteInfo> loadedNote = _db.loadNoteByID(noteID);
    if (loadedNote.has_value()) {
        return Note(std::move(loadedNote.value()), _db);
    }
    return std::nullopt;
}

}  // namespace core
