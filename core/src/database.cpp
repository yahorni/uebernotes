#include "database.hpp"

namespace uebernotes {

const char Database::_defaultDatabaseName[]{"db.sqlite3"};

Database::Database()
    : Database(_defaultDatabaseName) {}

Database::Database(std::string_view dbName)
    : _dbName(dbName),
      _dbStorage(initStorage(dbName)) {
    _dbStorage.sync_schema();
}

BookID Database::storeBook(const BookInfo& book) { return _dbStorage.insert(book); }

NoteID Database::storeNote(const NoteInfo& note) { return _dbStorage.insert(note); }

BookInfo Database::getBookInfoByID(BookID bookID) { return _dbStorage.get<BookInfo>(bookID); }

NoteInfo Database::getNoteInfoByID(NoteID noteID) { return _dbStorage.get<NoteInfo>(noteID); }

NotesInfoCollection Database::getNotesByBookID(BookID bookID) {
    auto notes = _dbStorage.get_all<NoteInfo>(sql::where(sql::is_equal(&NoteInfo::bookID, bookID)));
    return {notes.begin(), notes.end()};
}

} // namespace uebernotes
