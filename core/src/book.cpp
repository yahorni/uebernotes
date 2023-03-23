#include "core/book.hpp"

#include "core/storage.hpp"

namespace core {

// BookInfo
BookInfo::BookInfo(std::string&& name)
    : BookInfo(0, std::move(name)) {}

BookInfo::BookInfo(BookID id, std::string&& name)
    : id(id),
      name(std::move(name)) {}

// Book
Book::Book(BookInfo book, Storage& storage)
    : _book(std::move(book)),
      _storage(storage),
      _notes(_storage.getNotesInfoByBookID(_book.id)) {}

const BookInfo& Book::getBookInfo() const { return _book; }
const NotesInfoCollection& Book::getNotesInfo() const { return _notes; }

std::optional<NoteID> Book::createNote(NoteInfo&& note) { return _storage.createNote(_book, std::move(note)); }

std::optional<Note> Book::getNote(NoteID noteID) const { return _storage.getNote(noteID); }

}  // namespace core
