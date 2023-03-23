#include "core/storage.hpp"

namespace core {

Storage::Storage(const AppContext& context)
    : _db(context.database) {
    _booksInfo = _db.loadBooks();

    if (!context.hasInitialOperation) {
        initializeCache();
    }
}

void Storage::initializeCache() {
    moveNotesToCache(_db.loadNotes());
    for (const auto& book : _booksInfo) {
        _booksWithCachedNotes.emplace(book.id);
    }
}

void Storage::moveNotesToCache(NotesInfoCollection&& notes) {
    for (auto&& note : notes) {
        _notesInfo.emplace(std::make_shared<NoteInfo>(std::move(*note)));
    }
}

/*** books ***/

const BooksInfoCollection& Storage::getBooksInfo() const { return _booksInfo; }

std::optional<BookID> Storage::createBook(BookInfo&& book) {
    BookID id = _db.storeBook(book);                              // 1st DB call
    std::optional<BookInfo> insertedBook = _db.loadBookByID(id);  // 2nd DB call
    if (insertedBook.has_value()) {
        _booksInfo.emplace_back(std::move(insertedBook.value()));
        return id;
    }
    return std::nullopt;
}

std::optional<Book> Storage::getBook(BookID bookID) {
    std::optional<BookInfo> loadedBook = _db.loadBookByID(bookID);  // 1st DB call
    // TODO: try to optimize conversion
    if (loadedBook.has_value()) {
        // TODO: check RVO
        return Book(std::move(loadedBook.value()), *this);
    }
    return std::nullopt;
}

void Storage::removeBook(BookID) { printf("TODO: implement Storage::removeBook()\n"); }

/*** notes ***/

NotesInfoCollection Storage::getNotesInfoByBookID(BookID bookID) {
    if (!_booksWithCachedNotes.count(bookID)) {
        moveNotesToCache(_db.loadNotesByBookID(bookID));
        _booksWithCachedNotes.emplace(bookID);
    }

    NotesInfoCollection notes;
    for (const auto& note : _notesInfo) {
        if (note->bookID == bookID) {
            notes.emplace_back(note);
        }
    }
    return notes;
}

std::optional<NoteID> Storage::createNote(const BookInfo& book, NoteInfo&& note) {
    note.bookID = book.id;
    NoteID id = _db.storeNote(note);                              // 1st DB call
    std::optional<NoteInfo> insertedNote = _db.loadNoteByID(id);  // 2nd DB call
    if (insertedNote.has_value()) {
        _notesInfo.emplace(std::make_shared<NoteInfo>(std::move(insertedNote.value())));
        return id;
    }
    return std::nullopt;
}

std::optional<Note> Storage::getNote(NoteID noteID) {
    std::optional<NoteInfo> loadedNote = _db.loadNoteByID(noteID);
    if (loadedNote.has_value()) {
        return Note(std::move(loadedNote.value()), *this);
    }
    return std::nullopt;
}

void Storage::removeNote(NoteID) { printf("TODO: implement Storage::removeNote()\n"); }

}  // namespace core
