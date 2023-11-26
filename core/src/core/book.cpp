#include "core/book.hpp"

namespace core {

// BookInfo
BookInfo::BookInfo(std::string&& name)
    : BookInfo(0, std::move(name)) {}

BookInfo::BookInfo(BookID id, std::string&& name)
    : id(id),
      name(std::move(name)) {}

// Book
Book::Book(std::shared_ptr<BookInfo> book)
    : _book(std::move(book)) {}

const std::string& Book::getName() const { return _book->name; }

}  // namespace core
