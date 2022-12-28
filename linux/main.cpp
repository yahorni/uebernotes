#include <iostream>

#include "linux/client.hpp"

int main() {
    linux::Client client;
    auto& storage = client.getStorage();

    // filling book name
    uebernotes::BookInfo bookInfo{"book1"};
    //
    auto bookID = storage.createBook(std::move(bookInfo));
    auto book = storage.getBook(bookID);
    // opening editor and filling content for noteInfo
    uebernotes::NoteInfo noteInfo1{"note1", "content1"};
    uebernotes::NoteInfo noteInfo2{"note2", "content2"};
    //
    book.createNote(std::move(noteInfo1));
    book.createNote(std::move(noteInfo2));
    // show notes in book
    const auto& notes = book.getNotesInfo();
    std::cout << "Notes amount: " << notes.size() << std::endl;
    // ---
    // get books to display
    // BookInfo has BookID
    const auto& books = storage.getBooksInfo();
    // displaying all books
    std::cout << "Loaded books amount: " << books.size() << std::endl;
    //
    // choosing one book by bookID
    // get book notes to display
    // NoteInfo has NoteID
    auto loadedBook = storage.getBook(bookID);
    loadedBook.loadNotes();
    const auto& loadedNotes = loadedBook.getNotesInfo();
    // displaying all notes
    std::cout << "Loaded notes amount: " << loadedNotes.size() << std::endl;
    //
    // choosing one note by noteID
    uebernotes::NoteID noteID = 1;
    //
    auto note = book.getNote(noteID);
    // opening editor and modifying content
    std::string newNoteContent = "new_content";
    //
    note.updateContent(std::move(newNoteContent));

    return 0;
}
