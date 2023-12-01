#include <catch2/catch_test_macros.hpp>

#include "core/book.hpp"
#include "core/config.hpp"
#include "core/note.hpp"
#include "core/storage.hpp"

#include <filesystem>
#include <string>

// TODO: add logger with cout

core::BookID createBook(core::Storage& storage, std::string name) {
    auto bookID = storage.createBook(core::BookInfo{std::move(name)});
    REQUIRE(bookID.has_value());
    return bookID.value();
}

core::NoteID createNote(core::Storage& storage, core::BookID bookID, std::string&& content) {
    auto noteID = storage.createNote(core::NoteInfo{bookID, std::move(content)});
    REQUIRE(noteID.has_value());
    return noteID.value();
}

TEST_CASE("books", "[core.storage.cache]") {
    namespace fs = std::filesystem;
    const auto database = "testdb-core.sqlite3";

    // remove old database
    if (fs::exists(database)) {
        REQUIRE(fs::remove(database));
    }

    // TODO: get rid of direct storage usage in common tests
    core::Storage storage{core::Config{database, true}};

    SECTION("book with notes creation") {
        auto bookID = createBook(storage, "book1");

        // creating notes
        createNote(storage, bookID, "content1");
        createNote(storage, bookID, "content2");

        // show notes in book
        const auto& notes = storage.getNoteInfosByBookID(bookID);
        REQUIRE(notes.size() == 2);

        // get books to display
        const auto& books = storage.getBookInfos();
        // displaying all books
        REQUIRE(books.size() == 1);
    }

    SECTION("book update") {
        std::string oldName = "oldBookName1";
        std::string newName = "newBookName1";

        auto bookID = createBook(storage, oldName);
        auto book = storage.getBook(bookID);

        REQUIRE(book->getName() == oldName);
        storage.updateBook(bookID, std::string(newName));
        REQUIRE(book->getName() == newName);
    }

    SECTION("several books creation") {
        // filling several books
        size_t booksAmount = 5;
        for (size_t i = 0; i < booksAmount; i++) {
            auto bookID = createBook(storage, "book" + std::to_string(i + 1));
            for (size_t j = 0; j < i + 1; j++) {
                createNote(storage, bookID, "note" + std::to_string(j));
            }
        }

        const auto& bookInfos = storage.getBookInfos();
        REQUIRE(bookInfos.size() == booksAmount);
        size_t notesAmount = 0;
        for (const auto& bookInfo : bookInfos) {
            notesAmount++;
            const auto& notes = storage.getNoteInfosByBookID(bookInfo->id);
            REQUIRE(notes.size() == notesAmount);
        }
    }

    SECTION("note update") {
        auto bookID = createBook(storage, "book1");

        // creating notes
        auto noteID1 = createNote(storage, bookID, "old_content1");
        createNote(storage, bookID, "old_content2");

        // checking notes
        const auto& loadedNotes = storage.getNoteInfosByBookID(bookID);
        REQUIRE(loadedNotes.size() == 2);

        // choosing one note by noteID
        auto note1 = storage.getNote(noteID1);

        // modifying content
        std::string newNoteContent = "new_content";
        storage.updateNote(noteID1, std::string(newNoteContent));
        REQUIRE(note1->getContent() == newNoteContent);
    }

    SECTION("get nonexistant book") {
        auto book = storage.getBook(999);
        REQUIRE(!book.has_value());
    }

    SECTION("get nonexistant note") {
        auto note = storage.getNote(999);
        REQUIRE(!note.has_value());
    }

    SECTION("remove book") {
        auto bookID1 = createBook(storage, "book1");
        auto bookID2 = createBook(storage, "book1");

        // creating notes
        createNote(storage, bookID1, "b1n1");
        createNote(storage, bookID1, "b1n2");
        createNote(storage, bookID2, "b2n1");
        createNote(storage, bookID2, "b2n2");
        createNote(storage, bookID2, "b2n3");

        // remove book
        REQUIRE(storage.getBookInfos().size() == 2);
        REQUIRE(storage.getNoteInfosByBookID(bookID1).size() == 2);
        REQUIRE(storage.getNoteInfosByBookID(bookID2).size() == 3);

        REQUIRE(storage.removeBook(bookID2) == true);

        REQUIRE(storage.getBookInfos().size() == 1);
        REQUIRE(storage.getNoteInfosByBookID(bookID1).size() == 2);
        REQUIRE(storage.getNoteInfosByBookID(bookID2).size() == 0);
    }
}
