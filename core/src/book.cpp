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
Book::Book(std::shared_ptr<BookInfo> book, Storage& storage)
    : _book(std::move(book)),
      _storage(storage) {}

const BookInfo& Book::getBookInfo() const { return *_book; }
NotesCache Book::getNotesInfo() const { return _storage.getNotesInfoByBookID(_book->id); }

std::optional<NoteID> Book::createNote(NoteInfo&& note) {
    note.bookID = _book->id;
    if (auto id = _storage.createNote(std::move(note)); id.has_value()) {
        return *id;
    }
    return std::nullopt;
}

std::optional<Note> Book::getNote(NoteID noteID) const { return _storage.getNote(noteID); }

}  // namespace core
