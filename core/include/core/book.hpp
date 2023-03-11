#pragma once

#include "core/types.hpp"
#include "core/note.hpp"

#include <list>
#include <optional>
#include <string>
#include <string_view>

namespace core {

struct BookInfo {
    BookID id;
    std::string name;

    explicit BookInfo(std::string&& name = "");  // for manual creation
    BookInfo(BookID id, std::string&& name);     // for database
};

class Book {
public:
    Book(BookInfo book, Database& db);

    const BookInfo& getBookInfo() const;
    const NotesInfoCollection& getNotesInfo() const;

    void loadNotes();

    std::optional<NoteID> createNote(NoteInfo&& note);
    std::optional<Note> getNote(NoteID noteID) const;

private:
    BookInfo _book;
    Database& _db;
    NotesInfoCollection _notes;
};

using BooksInfoCollection = std::list<BookInfo>;

};  // namespace core
