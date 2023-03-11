#include "core/storage.hpp"

namespace core {

Storage::Storage(const Config& cfg)
    : _db(cfg.database) {
    _booksInfo = _db.loadBooks();
}

const BooksInfoCollection& Storage::getBooksInfo() const { return _booksInfo; }

std::optional<BookID> Storage::createBook(BookInfo&& book) {
    BookID id = _db.storeBook(book);                              // 1st DB call
    std::optional<BookInfo> insertedBook = _db.loadBookByID(id);  // 2nd DB call
    if (insertedBook.has_value()) {
        _booksInfo.emplace_back(std::move(insertedBook.value()));
        return id;
    }
    return std::nullopt;
}

std::optional<Book> Storage::getBook(BookID bookID) {
    std::optional<BookInfo> loadedBook = _db.loadBookByID(bookID);  // 1st DB call
    // TODO: try to optimize conversion
    if (loadedBook.has_value()) {
        // TODO: check RVO
        return Book(std::move(loadedBook.value()), _db);
    }
    return std::nullopt;
}

void Storage::removeBook(BookID) { printf("TODO: implement Storage::removeBook()\n"); }

}  // namespace core
