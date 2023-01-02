#include <catch2/catch_test_macros.hpp>

#include <iostream>

#include "core/book.hpp"
#include "core/note.hpp"
#include "core/storage.hpp"

TEST_CASE("smoke", "[core]") {
    core::Storage storage{core::Config{"testdb.sqlite3"}};
    // filling book name
    core::BookInfo bookInfo{"book1"};
    //
    auto bookID = storage.createBook(std::move(bookInfo));
    auto book = storage.getBook(bookID);
    // opening editor and filling content for noteInfo
    core::NoteInfo noteInfo1{"note1", "content1"};
    core::NoteInfo noteInfo2{"note2", "content2"};
    //
    book.createNote(std::move(noteInfo1));
    book.createNote(std::move(noteInfo2));
    // show notes in book
    const auto& notes = book.getNotesInfo();
    std::cout << "Notes amount: " << notes.size() << std::endl;
    REQUIRE(notes.size() == 2);
    // ---
    // get books to display
    // BookInfo has BookID
    const auto& books = storage.getBooksInfo();
    // displaying all books
    std::cout << "Loaded books amount: " << books.size() << std::endl;
    REQUIRE(books.size() == 1);
    //
    // choosing one book by bookID
    // get book notes to display
    // NoteInfo has NoteID
    auto loadedBook = storage.getBook(bookID);
    loadedBook.loadNotes();
    const auto& loadedNotes = loadedBook.getNotesInfo();
    // displaying all notes
    std::cout << "Loaded notes amount: " << loadedNotes.size() << std::endl;
    REQUIRE(loadedNotes.size() == 2);
    //
    // choosing one note by noteID
    core::NoteID noteID = 1;
    //
    auto note = book.getNote(noteID);
    // opening editor and modifying content
    std::string newNoteContent = "new_content";
    //
    note.updateContent(std::move(newNoteContent));
    // REQUIRE(note.getContent() == newNoteContent);
}
