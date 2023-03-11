#pragma once

#include "core/book.hpp"
#include "core/config.hpp"
#include "core/database.hpp"
#include "core/note.hpp"

#include <list>
#include <optional>

namespace core {

class Storage {
public:
    explicit Storage(const Config& cfg);

    // TODO: should I really restrict it explicitly (what about moving?) -- yes
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    const BooksInfoCollection& getBooksInfo() const;

    std::optional<BookID> createBook(BookInfo&& book);
    std::optional<Book> getBook(BookID bookID);
    void removeBook(BookID bookID);

private:
    Database _db;
    BooksInfoCollection _booksInfo;
};

}  // namespace core
