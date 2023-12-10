#include "linux/cli.hpp"

#include <core/config.hpp>

#include <format>
#include <iostream>
#include <utility>

namespace linux {

CLI::CLI(const core::Config& config)
    : _storage(config) {}

bool CLI::run(const CmdLineArgs& args) {
    // TODO: add operation enum
    if (args.has("list-books")) {
        return listBooks();
    } else if (args.has("print-book")) {
        return printBook(args.getUInt64("print-book"));
    } else if (args.has("print-note")) {
        return printNote(args.getUInt64("print-note"));
    } else if (args.has("create-book")) {
        return createBook(args.getString("create-book"));
    } else if (args.has("create-note")) {
        return createNote(args.getUInt64("create-note"), args.getString("note-content"));
    } else if (args.has("update-book")) {
        return updateBook(args.getUInt64("update-book"), args.getString("book-name"));
    } else if (args.has("update-note")) {
        return updateNote(args.getUInt64("update-note"), args.getString("note-content"));
    } else if (args.has("remove-book")) {
        return removeBook(args.getUInt64("remove-book"));
    } else if (args.has("remove-note")) {
        return removeNote(args.getUInt64("remove-note"));
    }

    std::cerr << "No argument to process" << std::endl;
    return false;
}

bool CLI::listBooks() {
    const auto& books = _storage.getBookInfos();

    // TODO: add formatted table printing
    for (const auto& book : books) {
        std::cout << std::format("Book ID: {} | Name: {}", book->id, book->name) << std::endl;
    }

    return true;
}

bool CLI::printBook(core::BookID bookID) {
    if (auto book = _storage.loadBookInfo(bookID); book) {
        const auto& notes = _storage.getNoteInfosByBookID(bookID);

        // TODO: add formatted table printing
        for (const auto& note : notes) {
            auto header = note->getName();
            auto truncatedContent = header.substr(0, 50);
            std::cout << std::format("Note ID: {} | Content: {}", note->id, truncatedContent) << std::endl;
        }
        return true;
    }

    std::cerr << std::format("Book not found", bookID) << std::endl;
    return false;
}

bool CLI::printNote(core::NoteID noteID) {
    if (const auto note = _storage.loadNoteInfo(noteID); note) {
        std::cout << note->getContent() << std::endl;
        return true;
    }

    std::cerr << std::format("Note not found", noteID) << std::endl;
    return false;
}

bool CLI::createBook(std::string&& name) {
    // FIXME: do not accept names with newlines or empty names (add tests)
    core::BookInfo info{std::move(name)};

    if (auto bookID = _storage.createBook(std::move(info)); bookID.has_value()) {
        std::clog << std::format("New book ID: {}", bookID.value()) << std::endl;
        return true;
    }

    std::cerr << std::format("Failed to create book") << std::endl;
    return false;
}

bool CLI::createNote(core::BookID bookID, std::string&& content) {
    core::NoteInfo info{bookID, std::move(content)};

    if (auto book = _storage.loadBookInfo(bookID); book) {
        if (auto noteID = _storage.createNote(std::move(info)); noteID.has_value()) {
            std::clog << std::format("New note ID: {}", noteID.value()) << std::endl;
            return true;
        }
    }

    std::cerr << std::format("Failed to create note") << std::endl;
    return false;
}

bool CLI::updateBook(core::BookID bookID, std::string&& name) {
    if (_storage.updateBook(bookID, std::move(name))) {
        return true;
    }

    std::cerr << std::format("Failed to update book, ID {}", bookID) << std::endl;
    return false;
}

bool CLI::updateNote(core::NoteID noteID, std::string&& content) {
    if (_storage.updateNote(noteID, std::move(content))) {
        return true;
    }

    std::cerr << std::format("Failed to update note, ID {}", noteID) << std::endl;
    return false;
}

bool CLI::removeBook(core::BookID bookID) {
    if (_storage.removeBook(bookID)) {
        return true;
    }

    std::cerr << std::format("Failed to remove book: ID {}", bookID) << std::endl;
    return false;
}

bool CLI::removeNote(core::NoteID noteID) {
    if (_storage.removeNote(noteID)) {
        return true;
    }

    std::cerr << std::format("Failed to remove note: ID {}", noteID) << std::endl;
    return false;
}

}  // namespace linux
