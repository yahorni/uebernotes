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

void Client::createBook(std::string_view bookName) {
    core::BookInfo info{std::string(bookName)};  // TODO: optimize string passing
    auto bookID = _storage.createBook(std::move(info));
    std::cout << "New book ID: " << bookID.value() << std::endl;
}

void Client::createNote(core::BookID bookID, std::string_view noteName, std::string_view content) {
    core::NoteInfo info{std::string(noteName), std::string(content)};  // TODO: optimize string passing
    auto book = _storage.getBook(bookID);
    if (book.has_value()) {
        auto noteID = book->createNote(std::move(info));
        std::cout << "New note ID: " << noteID.value() << std::endl;
    }
}

}  // namespace linux
