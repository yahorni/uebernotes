#include "core/database.hpp"

namespace core {

Database::Database(std::string_view dbName)
    : _dbName(dbName),
      _dbStorage(initStorage(dbName)) {
    _dbStorage.sync_schema();
}

BookID Database::storeBook(const BookInfo& book) { return _dbStorage.insert(book); }

NoteID Database::storeNote(const NoteInfo& note) { return _dbStorage.insert(note); }

BookInfo Database::loadBookByID(BookID bookID) { return _dbStorage.get<BookInfo>(bookID); }

NoteInfo Database::loadNoteByID(NoteID noteID) { return _dbStorage.get<NoteInfo>(noteID); }

BooksInfoCollection Database::loadBooks() {
    auto books = _dbStorage.get_all<BookInfo>();
    return {books.begin(), books.end()};
}

NotesInfoCollection Database::loadNotesByBookID(BookID bookID) {
    auto notes = _dbStorage.get_all<NoteInfo>(sql::where(sql::is_equal(&NoteInfo::bookID, bookID)));
    return {notes.begin(), notes.end()};
}

} // namespace core
