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
class StorageCache {
public:
    explicit StorageCache(bool isActive);

    void initialize(BooksCache&& books, NotesCache&& notes);

    void addBook(BookInfo&& book);
    void addNote(NoteInfo&& note);
    void addNotes(const NotesCache& notes);

    const BooksCache& getBooks() const;
    NotesCache getNotesByBookID(BookID bookID) const;

    std::shared_ptr<BookInfo> getBook(BookID bookID) const;
    std::shared_ptr<NoteInfo> getNote(NoteID noteID) const;

    void removeBook(BookID bookID);
    void removeNote(NoteID noteID);

    void replaceBooks(BooksCache&& books);

    const bool isActive;

private:
    // TODO: optimize cache data structure (maybe use map instead of set)
    BooksCache _booksCache;
    NotesCache _notesCache;
};

class Storage {
public:
    explicit Storage(const Config& config);
    ~Storage();

    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;
    Storage(Storage&&) = delete;
    Storage& operator=(Storage&&) = delete;

    BooksCache getBookInfos(bool skipCache = false) const;
    NotesCache getNoteInfosByBookID(BookID bookID, bool skipCache = false) const;

    std::optional<BookID> createBook(BookInfo&& book);
    std::optional<Book> getBook(BookID bookID);
    bool updateBook(BookID bookID, std::string&& name);
    bool removeBook(BookID bookID);

    std::optional<NoteID> createNote(NoteInfo&& note);
    std::optional<Note> getNote(NoteID noteID) const;
    bool updateNote(NoteID noteID, std::string&& content);
    bool removeNote(NoteID noteID);

private:
    std::shared_ptr<BookInfo> loadBookInfo(BookID bookID) const;
    std::shared_ptr<NoteInfo> loadNoteInfo(NoteID noteID) const;

    std::unique_ptr<Database> _db;

    mutable StorageCache _cache;
};

}  // namespace core
