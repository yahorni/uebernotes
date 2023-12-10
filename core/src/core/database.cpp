#include "core/database.hpp"

#include "core/logger.hpp"

#include <utility>

namespace core {

Database::Database(std::string_view dbName)
    : _dbName(dbName),
      _dbStorage(initStorage(dbName)) {
    Log::info("Initializing database: {}", dbName);
    _dbStorage.sync_schema();
}

std::optional<BookID> Database::insertBook(const Book& book) {
    try {
        return _dbStorage.insert(book);
    } catch (const std::system_error& ex) {
        Log::error("Failed to insert book: bookID = {}, code = {}, message = {}", book.id, ex.code().value(),
                   ex.what());
    }
    return std::nullopt;
}

std::optional<NoteID> Database::insertNote(const Note& note) {
    try {
        return _dbStorage.insert(note);
    } catch (const std::system_error& ex) {
        Log::error("Failed to insert note: noteID = {}, code = {}, message = {}", note.id, ex.code().value(),
                   ex.what());
    }
    return std::nullopt;
}

bool Database::hasBook(BookID bookID) {
    // TODO: handle db exceptions
    return _dbStorage.count<Book>(sql::where(sql::is_equal(&Book::id, bookID)));
}

bool Database::hasNote(NoteID noteID) {
    // TODO: handle db exceptions
    return _dbStorage.count<Note>(sql::where(sql::is_equal(&Note::id, noteID)));
}

BookPtr Database::loadBookByID(BookID bookID) {
    try {
        // TODO: use get_pointer with std::unique_ptr to get rid of exception
        return std::make_shared<Book>(_dbStorage.get<Book>(bookID));
    } catch (std::system_error& ex) {
        Log::error("Failed to get book: bookID = {}, code = {}, message = {}", bookID, ex.code().value(), ex.what());
    }
    return nullptr;
}

NotePtr Database::loadNoteByID(NoteID noteID) {
    try {
        // TODO: use get_pointer with std::unique_ptr to get rid of exception
        return std::make_shared<Note>(_dbStorage.get<Note>(noteID));
    } catch (std::system_error& ex) {
        Log::error("Failed to get note: noteID = {}, code = {}, message = {}", noteID, ex.code().value(), ex.what());
    }
    return nullptr;
}

bool Database::updateBook(BookID bookID, const std::string& name) {
    if (!hasBook(bookID)) {
        return false;
    }

    _dbStorage.update_all(sql::set(sql::c(&Book::name) = name),  //
                          sql::where(sql::is_equal(&Book::id, bookID)));
    return true;
}

bool Database::updateNote(NoteID noteID, const std::string& content) {
    if (!hasNote(noteID)) {
        return false;
    }

    _dbStorage.update_all(sql::set(sql::c(&Note::content) = content),
                          sql::where(sql::is_equal(&Note::id, noteID)));
    return true;
}

bool Database::removeBook(BookID bookID) {
    try {
        _dbStorage.remove<Book>(bookID);
        return true;
    } catch (std::system_error& ex) {
        Log::error("Failed to remove book: bookID = {}, code = {}, message = {}", bookID, ex.code().value(), ex.what());
    }
    return false;
}

bool Database::removeNote(NoteID noteID) {
    try {
        _dbStorage.remove<Note>(noteID);
        return true;
    } catch (std::system_error& ex) {
        Log::error("Failed to remove note: noteID = {}, code = {}, message = {}", noteID, ex.code().value(), ex.what());
    }
    return false;
}

BooksCache Database::loadBooks() {
    auto dbBooks = _dbStorage.get_all<Book>();
    BooksCache books;
    for (auto& book : dbBooks) {
        books.emplace(std::make_shared<Book>(std::move(book)));
    }
    return books;
}

template<class T>
static NotesCache convertDBNotesToCollection(T&& dbNotes) {
    NotesCache notes;
    for (auto& note : dbNotes) {
        notes.emplace(std::make_shared<Note>(std::move(note)));
    }
    return notes;
}

NotesCache Database::loadAllNotes() {
    auto notes = _dbStorage.get_all<Note>();
    return convertDBNotesToCollection<decltype(notes)>(std::move(notes));
}

NotesCache Database::loadNotesByBookID(BookID bookID) {
    auto notes = _dbStorage.get_all<Note>(sql::where(sql::is_equal(&Note::bookID, bookID)));
    return convertDBNotesToCollection<decltype(notes)>(std::move(notes));
}

}  // namespace core
