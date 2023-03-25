#include <catch2/catch_test_macros.hpp>

#include "core/appcontext.hpp"
#include "core/book.hpp"
#include "core/note.hpp"
#include "core/storage.hpp"

#include <filesystem>
#include <iostream>
#include <string>

core::BookID createBook(core::Storage& storage, std::string name) {
    core::BookInfo bookInfo{std::move(name)};
    auto bookID = storage.createBook(std::move(bookInfo));
    REQUIRE(bookID.has_value());
    return bookID.value();
}

core::NoteID createNote(core::Book& book, std::string&& content) {
    auto noteID = book.createNote(core::NoteInfo{content});
    REQUIRE(noteID.has_value());
    return noteID.value();
}

TEST_CASE("books", "[core.storage.cache]") {
    namespace fs = std::filesystem;
    const auto database = "testdb-core.sqlite3";

    // remove old database
    if (fs::exists(database)) REQUIRE(fs::remove(database));

    // TODO: get rid of direct storage usage in common tests
    core::Storage storage{core::AppContext{database, true}};

    SECTION("book with notes creation") {
        auto bookID = createBook(storage, "book1");
        auto book = storage.getBook(bookID);

        // creating notes
        createNote(*book, "content1");
        createNote(*book, "content2");

        // show notes in book
        const auto& notes = book->getNotesInfo();
        REQUIRE(notes.size() == 2);

        // get books to display
        const auto& books = storage.getBooksInfo();
        // displaying all books
        REQUIRE(books.size() == 1);
    }

    SECTION("book update") {
        std::string oldName = "oldBookName1";
        std::string newName = "newBookName1";

        auto bookID = createBook(storage, oldName);
        auto book = storage.getBook(bookID);

        REQUIRE(book->getBookInfo().name == oldName);
        storage.updateBook(bookID, std::string(newName));
        REQUIRE(book->getBookInfo().name == newName);
    }

    SECTION("several books creation") {
        // filling several books
        size_t booksAmount = 5;
        for (size_t i = 0; i < booksAmount; i++) {
            auto bookID = createBook(storage, "book" + std::to_string(i + 1));
            auto book = storage.getBook(bookID);

            for (size_t j = 0; j < i + 1; j++) {
                createNote(*book, "note" + std::to_string(j));
            }
        }

        const auto& bookInfos = storage.getBooksInfo();
        REQUIRE(bookInfos.size() == booksAmount);
        size_t notesAmount = 0;
        for (const auto& bookInfo : bookInfos) {
            notesAmount++;
            auto book = storage.getBook(bookInfo->id);
            const auto& notes = book->getNotesInfo();
            REQUIRE(notes.size() == notesAmount);
        }
    }

    SECTION("note update") {
        auto bookID = createBook(storage, "book1");
        auto book = storage.getBook(bookID);

        // creating notes
        auto noteID1 = createNote(*book, "content1");
        createNote(*book, "content2");

        // choosing one book by bookID
        // get book notes to display
        auto loadedBook = storage.getBook(bookID);
        const auto& loadedNotes = loadedBook->getNotesInfo();

        // displaying all notes
        REQUIRE(loadedNotes.size() == 2);

        // choosing one note by noteID
        auto note1 = book->getNote(noteID1);
        // opening editor and modifying content
        std::string newNoteContent = "new_content";
        note1->updateContent(std::string(newNoteContent));
        REQUIRE(note1->getContent() == newNoteContent);
        // check data integrity in storage
        auto note1Storage = storage.getNote(noteID1);
        REQUIRE(note1Storage->getContent() == newNoteContent);
    }

    SECTION("get nonexistant book") {
        auto book = storage.getBook(999);
        REQUIRE(!book.has_value());
    }

    SECTION("get nonexistant note") {
        auto note = storage.getNote(999);
        REQUIRE(!note.has_value());
    }
}
