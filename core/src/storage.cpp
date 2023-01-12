#include "core/storage.hpp"

namespace core {

Storage::Storage(const Config& cfg)
    : _db(cfg.database) {
    _booksInfo = _db.loadBooks();
}

const BooksInfoCollection& Storage::getBooksInfo() const { return _booksInfo; }

BookID Storage::createBook(BookInfo&& book) {
    BookID id = _db.storeBook(book);               // 1st DB call
    BookInfo insertedBook = _db.loadBookByID(id);  // 2nd DB call
    _booksInfo.emplace_back(insertedBook);
    return id;
}

Book Storage::getBook(BookID bookID) {
    BookInfo insertedBook = _db.loadBookByID(bookID);  // 1st DB call
    // TODO: check RVO
    return Book(insertedBook, _db);
}

void Storage::removeBook(BookID) { printf("TODO: implement Storage::removeBook()\n"); }

}  // namespace core
