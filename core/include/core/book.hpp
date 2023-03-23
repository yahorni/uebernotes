#pragma once

#include "core/note.hpp"
#include "core/types.hpp"

#include <list>
#include <optional>
#include <string>
#include <string_view>

namespace core {

class Storage;

struct BookInfo {
    BookID id;
    std::string name;

    explicit BookInfo(std::string&& name = "");  // for manual creation
    BookInfo(BookID id, std::string&& name);     // for database
};

class Book {
public:
    Book(BookInfo book, Storage& storage);

    const BookInfo& getBookInfo() const;
    const NotesInfoCollection& getNotesInfo() const;

    std::optional<NoteID> createNote(NoteInfo&& note);
    std::optional<Note> getNote(NoteID noteID) const;

private:
    BookInfo _book;
    Storage& _storage;
    NotesInfoCollection _notes;
};

using BooksInfoCollection = std::list<BookInfo>;

};  // namespace core
