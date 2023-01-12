#include "linux/client.hpp"

#include <iostream>

namespace linux {

Client::Client(const core::Config& cfg)
    : _storage(cfg) {}

core::Storage& Client::getStorage() { return _storage; }

void Client::listBooks() {
    const auto& books = _storage.getBooksInfo();
    std::cout << "Amount of books: " << books.size() << std::endl;
    for (const auto& book : books) {
        std::cout << "ID: " << book.id << "; Name: " << book.name << std::endl;
    }
}

void Client::createBook(std::string_view book_name) {
    core::BookInfo info{std::string(book_name)};
    auto bookID = _storage.createBook(std::move(info));
    std::cout << "New book ID: " << bookID << std::endl;
}

}  // namespace linux
