#include "book.hpp"

#include "database.hpp"

namespace uebernotes {

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

void Book::loadNotes() {
    _notes = _db.getNotesByBookID(_book.id);
}

// void Book::createNote(const NoteInfo& note) { _notes.push_back(note); }
NoteID Book::createNote(NoteInfo&& note) {
    note.bookID = _book.id;
    NoteID id = _db.storeNote(note);                 // 1st DB call
    NoteInfo insertedNote = _db.getNoteInfoByID(id); // 2nd DB call
    _notes.emplace_back(std::move(insertedNote));
    return id;
}

Note Book::getNote(NoteID noteID) const {
    auto noteInfo = _db.getNoteInfoByID(noteID);
    return Note(noteInfo, _db);
}

} // namespace uebernotes
