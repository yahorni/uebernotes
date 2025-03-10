#pragma once

#include "core/book.hpp"
#include "core/config.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

#include <memory>
#include <optional>
#include <string>

namespace core {

class Database;

// TODO:
// 1. make StorageCache private
// 2. rewrite cache to work with database which can be changed outside
// (implement cache miss properly)
class StorageCache {
public:
    explicit StorageCache(bool isActive);

    void initialize(BooksCache&& books, NotesCache&& notes);

    void addBook(Book&& book);
    void addNote(Note&& note);
    void addNotes(const NotesCache& notes);

    const BooksCache& getBooks() const;
    NotesCache getNotesByBookID(BookID bookID) const;

    BookPtr getBook(BookID bookID) const;
    NotePtr getNote(NoteID noteID) const;

    void removeBook(BookID bookID);
    void removeNotesForBook(BookID bookID);
    void removeNote(NoteID noteID);

    const bool isActive;

private:
    // TODO: optimize cache data structure (maybe use map instead of set)
    BooksCache _booksCache;
    NotesCache _notesCache;
};

class Storage {
public:
    explicit Storage(const Config& config);
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;
    Storage(Storage&&) = delete;
    Storage& operator=(Storage&&) = delete;
    ~Storage();

    void load();

    void loadBooksToCache();
    void loadNotesToCache(BookID bookID);

    BooksCache getBooks() const;
    NotesCache getNotesByBookID(BookID bookID) const;

    std::optional<BookID> createBook(Book&& book);
    BookPtr loadBook(BookID bookID) const;
    bool updateBook(BookID bookID, std::string&& name);
    bool removeBook(BookID bookID);

    std::optional<NoteID> createNote(Note&& note);
    NotePtr loadNote(NoteID noteID) const;
    bool updateNote(NoteID noteID, std::string&& content);
    bool removeNote(NoteID noteID);

private:
    std::unique_ptr<Database> _db;
    mutable StorageCache _cache;
};

}  // namespace core
