#pragma once

#include <list>

#include "book.hpp"
#include "database.hpp"
#include "note.hpp"

namespace uebernotes {

class Storage {
public:
    Storage();
    Storage(const Storage&) = delete; // is it really needed?
    // TODO: restrict copying (and moving?)

    const BooksInfoCollection& getBooksInfo() const;

    BookID createBook(BookInfo&& book);
    Book getBook(BookID bookID);
    void removeBook(BookID bookID);

private:
    Database _db;
    BooksInfoCollection _booksInfo;
};

} // namespace uebernotes
