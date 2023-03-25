#include "linux/client.hpp"

#include "core/appcontext.hpp"

#include <iostream>

namespace linux {

Client::Client(const core::AppContext& context)
    : _storage(context) {}

void Client::listBooks() {
    const auto& books = _storage.getBooksInfo();
    std::cout << "Amount of books: " << books.size() << std::endl;
    for (const auto& book : books) {
        std::cout << "ID: " << book->id  //
                  << " | Name: " << book->name << std::endl;
    }
}

void Client::printBook(core::BookID bookID) {
    if (auto book = _storage.getBook(bookID); book.has_value()) {
        const auto& notes = _storage.getNotesInfoByBookID(bookID);
        std::cout << "Amount of notes: " << notes.size() << std::endl;
        for (const auto& note : notes) {
            auto truncated_content = std::string_view(note->content).substr(0, 50);

            std::cout << "ID: " << note->id                   //
                      << " | Book ID: " << note->bookID       //
                      << " | Content: " << truncated_content  //
                      << " |" << std::endl;
        }
    }
}

void Client::printNote(core::NoteID noteID) {
    if (const auto note = _storage.getNote(noteID); note.has_value()) {
        std::cout << "Content: " << std::endl << note->getContent() << std::endl;
    }
}

void Client::createBook(std::string&& name) {
    core::BookInfo info{std::move(name)};
    auto bookID = _storage.createBook(std::move(info));
    std::cout << "New book ID: " << bookID.value() << std::endl;
}

void Client::createNote(core::BookID bookID, std::string&& content) {
    core::NoteInfo info{bookID, std::move(content)};
    if (auto book = _storage.getBook(bookID); book.has_value()) {
        auto noteID = _storage.createNote(std::move(info));
        std::cout << "New note ID: " << noteID.value() << std::endl;
    }
}

void Client::updateBook(core::BookID bookID, std::string&& name) { _storage.updateBook(bookID, std::move(name)); }

void Client::updateNote(core::NoteID noteID, std::string&& content) { _storage.updateNote(noteID, std::move(content)); }

void Client::removeBook(core::BookID bookID) { _storage.removeBook(bookID); }

void Client::removeNote(core::NoteID noteID) { _storage.removeNote(noteID); }

}  // namespace linux
