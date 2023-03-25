#pragma once

#include "core/comparator.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

#include <list>
#include <memory>
#include <optional>
#include <set>
#include <string>

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
    Book(std::shared_ptr<BookInfo> book, Storage& storage);

    const BookInfo& getBookInfo() const;
    NotesCache getNotesInfo() const;

    std::optional<NoteID> createNote(NoteInfo&& note);
    std::optional<Note> getNote(NoteID noteID) const;

private:
    std::shared_ptr<BookInfo> _book;
    Storage& _storage;
};

using BooksCache = std::set<std::shared_ptr<BookInfo>, SharedPtrWithID::Compare>;

};  // namespace core
