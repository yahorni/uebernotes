#include "core/storage.hpp"

#include "core/database.hpp"

#include <algorithm>

namespace core {

/*** cache ***/

StorageCache::StorageCache(bool isActive)
    : isActive(isActive) {}

void StorageCache::initialize(BooksCache&& books, NotesCache&& notes) {
    _booksCache = std::move(books);
    _notesCache = std::move(notes);
}

void StorageCache::addBook(Book&& book) { _booksCache.emplace(std::make_shared<Book>(std::move(book))); }

void StorageCache::addNote(Note&& note) { _notesCache.emplace(std::make_shared<Note>(std::move(note))); }

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

static inline auto findBookInCache(const BooksCache& booksCache, BookID bookID) {
    return std::find_if(booksCache.begin(), booksCache.end(),  //
                        [bookID](const auto& book) { return book->id == bookID; });
}

static inline auto findNoteInCache(const NotesCache& notesCache, NoteID noteID) {
    return std::find_if(notesCache.begin(), notesCache.end(),
                        [noteID](const auto& note) { return note->id == noteID; });
}

BookPtr StorageCache::getBook(BookID bookID) const {
    auto bookIter = findBookInCache(_booksCache, bookID);
    return bookIter != _booksCache.end() ? *bookIter : nullptr;
}

NotePtr StorageCache::getNote(NoteID noteID) const {
    auto noteIter = findNoteInCache(_notesCache, noteID);
    return noteIter != _notesCache.end() ? *noteIter : nullptr;
}

void StorageCache::removeBook(BookID bookID) {
    removeNotesForBook(bookID);

    auto bookIter = findBookInCache(_booksCache, bookID);
    _booksCache.erase(bookIter);
}

void StorageCache::removeNotesForBook(BookID bookID) {
    for (auto it = _notesCache.begin(); it != _notesCache.end();) {
        if ((*it)->bookID == bookID) {
            it = _notesCache.erase(it);
        } else {
            ++it;
        }
    }
}

void StorageCache::removeNote(NoteID noteID) {
    auto noteIter = findNoteInCache(_notesCache, noteID);
    _notesCache.erase(noteIter);
}

/*** storage ***/

Storage::Storage(const Config& config)
    : _db(new Database{config.database}),
      _cache(config.useCaching) {
    load();
}

// need to have definition here for database
Storage::~Storage() = default;

void Storage::load() {
    if (_cache.isActive) {
        loadBooksToCache();
    }
}

void Storage::loadBooksToCache() { _cache.initialize(_db->loadBooks(), _db->loadAllNotes()); }

void Storage::loadNotesToCache(BookID bookID) {
    auto notes = _db->loadNotesByBookID(bookID);
    _cache.removeNotesForBook(bookID);
    _cache.addNotes(notes);
}

BooksCache Storage::getBooks() const {
    if (_cache.isActive) {
        return _cache.getBooks();
    }
    return _db->loadBooks();
}

NotesCache Storage::getNotesByBookID(BookID bookID) const {
    if (_cache.isActive) {
        return _cache.getNotesByBookID(bookID);
    }
    return _db->loadNotesByBookID(bookID);
}

std::optional<BookID> Storage::createBook(Book&& book) {
    auto id = _db->insertBook(book);
    if (!id.has_value()) {
        return std::nullopt;
    }

    if (_cache.isActive) {
        book.id = *id;
        _cache.addBook(std::move(book));
    }
    return *id;
}

std::optional<NoteID> Storage::createNote(Note&& note) {
    auto id = _db->insertNote(note);
    if (!id.has_value()) {
        return std::nullopt;
    }

    if (_cache.isActive) {
        note.id = *id;
        _cache.addNote(std::move(note));
    }
    return *id;
}

BookPtr Storage::loadBook(BookID bookID) const {
    BookPtr bookPtr;
    if (_cache.isActive) {
        bookPtr = _cache.getBook(bookID);
    } else {
        bookPtr = _db->loadBookByID(bookID);
    }
    return bookPtr;
}

NotePtr Storage::loadNote(NoteID noteID) const {
    NotePtr notePtr;
    if (_cache.isActive) {
        notePtr = _cache.getNote(noteID);
    } else {
        notePtr = _db->loadNoteByID(noteID);
    }
    return notePtr;
}

bool Storage::updateBook(BookID bookID, std::string&& name) {
    if (auto bookPtr = loadBook(bookID); bookPtr && _db->updateBook(bookID, name)) {
        bookPtr->name = std::move(name);
        return true;
    }
    return false;
}

bool Storage::updateNote(NoteID noteID, std::string&& content) {
    if (auto notePtr = loadNote(noteID); notePtr && _db->updateNote(noteID, content)) {
        notePtr->content = std::move(content);
        return true;
    }
    return false;
}

bool Storage::removeBook(BookID bookID) {
    // DB removal doesn't return error on non-existant entity
    if (auto bookPtr = loadBook(bookID); !bookPtr || !_db->removeBook(bookID)) {
        return false;
    }
    if (_cache.isActive) {
        _cache.removeBook(bookID);
    }
    return true;
}

bool Storage::removeNote(NoteID noteID) {
    // DB removal doesn't return error on non-existant entity
    if (auto notePtr = loadNote(noteID); !notePtr || !_db->removeNote(noteID)) {
        return false;
    }
    if (_cache.isActive) {
        _cache.removeNote(noteID);
    }
    return true;
}

}  // namespace core
