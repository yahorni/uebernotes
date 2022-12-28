#include "storage.hpp"

namespace uebernotes {

Storage::Storage()
    : _db() {}

const BooksInfoCollection& Storage::getBooksInfo() const { return _booksInfo; }

BookID Storage::createBook(BookInfo&& book) {
    BookID id = _db.storeBook(book);                 // 1st DB call
    BookInfo insertedBook = _db.getBookInfoByID(id); // 2nd DB call
    _booksInfo.emplace_back(insertedBook);
    return id;
}

Book Storage::getBook(BookID bookID) {
    BookInfo insertedBook = _db.getBookInfoByID(bookID); // 1st DB call
    // TODO: check RVO
    return Book(insertedBook, _db);
}

void removeBook(BookID) { printf("TODO: implement Storage::removeBook()\n"); }

} // namespace uebernotes
