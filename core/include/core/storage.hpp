#pragma once

#include "core/appcontext.hpp"
#include "core/book.hpp"
#include "core/database.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_set>

namespace core {

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

    const bool isActive;

private:
    BooksCache _booksCache;
    NotesCache _notesCache;
};

class Storage {
public:
    explicit Storage(const AppContext& context);

    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;
    Storage(Storage&&) = delete;
    Storage& operator=(Storage&&) = delete;

    BooksCache getBooksInfo() const;
    NotesCache getNotesInfoByBookID(BookID bookID) const;

    std::optional<BookID> createBook(BookInfo&& book);
    std::optional<Book> getBook(BookID bookID);
    void updateBook(BookID bookID, std::string&& name);
    void removeBook(BookID bookID);

    std::optional<NoteID> createNote(NoteInfo&& note);
    std::optional<Note> getNote(NoteID noteID) const;
    void updateNote(NoteID noteID, std::string&& content);
    void removeNote(NoteID noteID);

private:
    std::shared_ptr<BookInfo> loadBookInfo(BookID bookID) const;
    std::shared_ptr<NoteInfo> loadNoteInfo(NoteID noteID) const;

    mutable Database _db;

    StorageCache _cache;
};

}  // namespace core
