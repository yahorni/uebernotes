#pragma once

#include <list>

#include "core/book.hpp"
#include "core/config.hpp"
#include "core/database.hpp"
#include "core/note.hpp"

namespace core {

class Storage {
public:
    explicit Storage(const Config& cfg);

    // should I really restrict it explicitly (what about moving?)
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    const BooksInfoCollection& getBooksInfo() const;

    BookID createBook(BookInfo&& book);
    Book getBook(BookID bookID);
    void removeBook(BookID bookID);

private:
    Database _db;
    BooksInfoCollection _booksInfo;
};

} // namespace core
