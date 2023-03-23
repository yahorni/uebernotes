#include "core/database.hpp"

#include <iostream>

namespace core {

Database::Database(std::string_view dbName)
    : _dbName(dbName),
      _dbStorage(initStorage(dbName)) {
    _dbStorage.sync_schema();
}

BookID Database::storeBook(const BookInfo& book) { return _dbStorage.insert(book); }

NoteID Database::storeNote(const NoteInfo& note) { return _dbStorage.insert(note); }

std::optional<BookInfo> Database::loadBookByID(BookID bookID) {
    try {
        return _dbStorage.get<BookInfo>(bookID);
    } catch (std::system_error& ex) {
        // TODO: log error
        std::cout << "Failed to get book: bookID = " << bookID << ", code = " << ex.code()
                  << ", message = " << ex.what() << std::endl;
    }
    return std::nullopt;
}

std::optional<NoteInfo> Database::loadNoteByID(NoteID noteID) {
    try {
        return _dbStorage.get<NoteInfo>(noteID);
    } catch (std::system_error& ex) {
        // TODO: log error
        std::cout << "Failed to get note: noteID = " << noteID << ", code = " << ex.code()
                  << ", message = " << ex.what() << std::endl;
    }
    return std::nullopt;
}

BooksInfoCollection Database::loadBooks() {
    auto books = _dbStorage.get_all<BookInfo>();
    return {books.begin(), books.end()};
}

template <class T> static NotesInfoCollection convertDBNotesToCollection(T&& dbNotes) {
    NotesInfoCollection notes;
    for (auto& note : dbNotes) {
        notes.emplace_back(std::make_shared<NoteInfo>(std::move(note)));  // TODO: ensure it's moved
    }
    return notes;
}

NotesInfoCollection Database::loadNotes() {
    auto notes = _dbStorage.get_all<NoteInfo>();
    return convertDBNotesToCollection<decltype(notes)>(std::move(notes));
}

NotesInfoCollection Database::loadNotesByBookID(BookID bookID) {
    auto notes = _dbStorage.get_all<NoteInfo>(sql::where(sql::is_equal(&NoteInfo::bookID, bookID)));
    return convertDBNotesToCollection<decltype(notes)>(std::move(notes));
}

}  // namespace core
