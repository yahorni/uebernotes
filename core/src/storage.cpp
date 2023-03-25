#include "core/storage.hpp"

namespace core {

/*** cache ***/

StorageCache::StorageCache(bool isActive)
    : isActive(isActive) {}

void StorageCache::initialize(BooksCache&& books, NotesCache&& notes) {
    _booksCache = std::move(books);
    _notesCache = std::move(notes);
}

void StorageCache::addBook(BookInfo&& book) { _booksCache.emplace(std::make_shared<BookInfo>(std::move(book))); }

void StorageCache::addNote(NoteInfo&& note) { _notesCache.emplace(std::make_shared<NoteInfo>(std::move(note))); }

void StorageCache::addNotes(const NotesCache& notes) {
    for (const auto& note : notes) {
        _notesCache.emplace(note);
    }
}

const BooksCache& StorageCache::getBooks() const { return _booksCache; }

NotesCache StorageCache::getNotesByBookID(BookID bookID) const {
    NotesCache notes;
    for (auto note : _notesCache) {
        if (note->bookID == bookID) {
            notes.emplace(note);
        }
    }
    return notes;
}

std::shared_ptr<BookInfo> StorageCache::getBook(BookID bookID) const {
    auto bookIter = std::find_if(_booksCache.begin(), _booksCache.end(),  //
                                 [bookID](const auto& book) { return book->id == bookID; });
    return bookIter != _booksCache.end() ? *bookIter : nullptr;
}

std::shared_ptr<NoteInfo> StorageCache::getNote(NoteID noteID) const {
    auto noteIter = std::find_if(_notesCache.begin(), _notesCache.end(),  //
                                 [noteID](const auto& note) { return note->id == noteID; });
    return noteIter != _notesCache.end() ? *noteIter : nullptr;
}

/*** storage ***/

Storage::Storage(const AppContext& context)
    : _db(context.database),
      _cache(context.useCaching) {
    if (_cache.isActive) {
        _cache.initialize(_db.loadBooks(), _db.loadAllNotes());
    }
}

BooksCache Storage::getBooksInfo() const {
    if (_cache.isActive) {
        return _cache.getBooks();
    }

    return _db.loadBooks();
}

NotesCache Storage::getNotesInfoByBookID(BookID bookID) const {
    if (_cache.isActive) {
        return _cache.getNotesByBookID(bookID);
    }

    return _db.loadNotesByBookID(bookID);
}

std::optional<BookID> Storage::createBook(BookInfo&& book) {
    auto id = _db.insertBook(book);
    if (!id.has_value()) {
        return std::nullopt;
    }

    if (_cache.isActive) {
        book.id = *id;
        _cache.addBook(std::move(book));
    }
    return *id;
}

std::optional<NoteID> Storage::createNote(NoteInfo&& note) {
    auto id = _db.insertNote(note);
    if (!id.has_value()) {
        return std::nullopt;
    }

    if (_cache.isActive) {
        note.id = *id;
        _cache.addNote(std::move(note));
    }
    return *id;
}

std::shared_ptr<BookInfo> Storage::loadBookInfo(BookID bookID) const {
    std::shared_ptr<BookInfo> bookPtr;
    if (_cache.isActive) {
        bookPtr = _cache.getBook(bookID);
    } else {
        bookPtr = _db.loadBookByID(bookID);
    }
    return bookPtr;
}

std::shared_ptr<NoteInfo> Storage::loadNoteInfo(NoteID noteID) const {
    std::shared_ptr<NoteInfo> notePtr;
    if (_cache.isActive) {
        notePtr = _cache.getNote(noteID);
    } else {
        notePtr = _db.loadNoteByID(noteID);
    }
    return notePtr;
}

std::optional<Book> Storage::getBook(BookID bookID) {
    auto bookPtr = loadBookInfo(bookID);
    return bookPtr ? std::make_optional(Book(bookPtr, *this)) : std::nullopt;
}

std::optional<Note> Storage::getNote(NoteID noteID) const {
    auto notePtr = loadNoteInfo(noteID);
    return notePtr ? std::make_optional(Note(notePtr)) : std::nullopt;
}

void Storage::updateBook(BookID bookID, std::string&& name) {
    if (auto bookPtr = loadBookInfo(bookID); bookPtr && _db.updateBook(bookID, name)) {
        bookPtr->name = std::move(name);
    }
}

void Storage::updateNote(NoteID noteID, std::string&& content) {
    if (auto notePtr = loadNoteInfo(noteID); notePtr && _db.updateNote(noteID, content)) {
        notePtr->content = std::move(content);
    }
}

void Storage::removeBook(BookID) { printf("TODO: implement Storage::removeBook()\n"); }

void Storage::removeNote(NoteID) { printf("TODO: implement Storage::removeNote()\n"); }

}  // namespace core
