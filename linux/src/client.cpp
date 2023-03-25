#include "linux/client.hpp"

#include "core/appcontext.hpp"

#include <iostream>

namespace linux {

Client::Client(const core::AppContext& context)
    : _storage(context) {}

bool Client::listBooks() {
    const auto& books = _storage.getBooksInfo();
    std::cout << "Amount of books: " << books.size() << std::endl;
    for (const auto& book : books) {
        std::cout << "ID: " << book->id  //
                  << " | Name: " << book->name << std::endl;
    }
    return true;
}

bool Client::printBook(core::BookID bookID) {
    if (auto book = _storage.getBook(bookID); !book.has_value()) {
        return false;
    }

    // TODO pass error here, without additional check above
    const auto& notes = _storage.getNotesInfoByBookID(bookID);
    std::cout << "Amount of notes: " << notes.size() << std::endl;
    for (const auto& note : notes) {
        auto truncated_content = std::string_view(note->content).substr(0, 50);

        std::cout << "ID: " << note->id                   //
                  << " | Book ID: " << note->bookID       //
                  << " | Content: " << truncated_content  //
                  << " |" << std::endl;
    }
    return true;
}

bool Client::printNote(core::NoteID noteID) {
    if (const auto note = _storage.getNote(noteID); note.has_value()) {
        std::cout << "Content: " << std::endl << note->getContent() << std::endl;
        return true;
    }
    return false;
}

bool Client::createBook(std::string&& name) {
    core::BookInfo info{std::move(name)};
    if (auto bookID = _storage.createBook(std::move(info)); bookID.has_value()) {
        std::cout << "New book ID: " << bookID.value() << std::endl;
        return true;
    }
    return false;
}

bool Client::createNote(core::BookID bookID, std::string&& content) {
    core::NoteInfo info{bookID, std::move(content)};
    if (auto book = _storage.getBook(bookID); book.has_value()) {
        auto noteID = _storage.createNote(std::move(info));
        std::cout << "New note ID: " << noteID.value() << std::endl;
        return true;
    }
    return false;
}

bool Client::updateBook(core::BookID bookID, std::string&& name) {
    if (!_storage.updateBook(bookID, std::move(name))) {
        std::cout << "Failed to update book" << std::endl;
        return false;
    }
    return true;
}

bool Client::updateNote(core::NoteID noteID, std::string&& content) {
    if (!_storage.updateNote(noteID, std::move(content))) {
        std::cout << "Failed to update note" << std::endl;
        return false;
    }
    return true;
}

bool Client::removeBook(core::BookID bookID) {
    if (!_storage.removeBook(bookID)) {
        std::cout << "Failed to remove book" << std::endl;
        return false;
    }
    return true;
}

bool Client::removeNote(core::NoteID noteID) {
    if (!_storage.removeNote(noteID)) {
        std::cout << "Failed to remove note" << std::endl;
        return false;
    }
    return true;
}

}  // namespace linux
