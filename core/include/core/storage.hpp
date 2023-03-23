#pragma once

#include "core/appcontext.hpp"
#include "core/book.hpp"
#include "core/database.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

#include <memory>
#include <optional>
#include <unordered_set>

namespace core {

using NotesCache = std::unordered_set<std::shared_ptr<NoteInfo>>;

class Storage {
public:
    explicit Storage(const AppContext& context);

    // TODO: should I really restrict it explicitly (what about moving?) -- yes
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    const BooksInfoCollection& getBooksInfo() const;
    NotesInfoCollection getNotesInfoByBookID(BookID bookID);

    std::optional<BookID> createBook(BookInfo&& book);
    std::optional<Book> getBook(BookID bookID);
    void removeBook(BookID bookID);

    std::optional<NoteID> createNote(const BookInfo& book, NoteInfo&& note);
    std::optional<Note> getNote(NoteID noteID);
    void removeNote(NoteID noteID);

private:
    void initializeCache();
    void moveNotesToCache(NotesInfoCollection&& notes);

    Database _db;

    BooksInfoCollection _booksInfo;
    std::unordered_set<BookID> _booksWithCachedNotes;
    NotesCache _notesInfo;
};

}  // namespace core
