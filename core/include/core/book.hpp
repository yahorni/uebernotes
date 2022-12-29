#pragma once

#include <list>
#include <string>
#include <string_view>

#include "core/common_types.hpp"
#include "core/note.hpp"

namespace uebernotes {

struct BookInfo {
    BookID id;
    std::string name;

    explicit BookInfo(std::string&& name = ""); // for manual creation
    BookInfo(BookID id, std::string&& name);    // for database
};

class Book {
public:
    Book(BookInfo book, Database& db);

    const BookInfo& getBookInfo() const;
    const NotesInfoCollection& getNotesInfo() const;

    void loadNotes();

    // void createNote(const NoteInfo& note);
    NoteID createNote(NoteInfo&& note);
    Note getNote(NoteID noteID) const;

private:
    BookInfo _book;
    Database& _db;
    NotesInfoCollection _notes;
};

using BooksInfoCollection = std::list<BookInfo>;

}; // namespace uebernotes
