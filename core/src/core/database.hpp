#pragma once

#include "core/book.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

#include <memory>
#include <string>
#include <string_view>

#include <sqlite_orm/sqlite_orm.h>
// https://github.com/fnc12/sqlite_orm/blob/master/examples/private_class_members.cpp

namespace core {

namespace sql = sqlite_orm;

inline auto initStorage(std::string_view _dbName) {
    return sql::make_storage(                                                                       //
        std::string(_dbName),                                                                       //
        sql::make_table("normal_books",                                                             //
                        sql::make_column("id", &Book::id, sql::primary_key().autoincrement()),  //
                        sql::make_column("name", &Book::name)),                                 //
        sql::make_table("normal_notes",                                                             //
                        sql::make_column("id", &Note::id, sql::primary_key().autoincrement()),  //
                        sql::make_column("book_id", &Note::bookID),                             //
                        sql::make_column("content", &Note::content),                            //
                        sql::foreign_key(&Note::bookID).references(&Book::id).on_delete.cascade()));
}

class Database {
public:
    explicit Database(std::string_view dbName);

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    std::optional<BookID> insertBook(const Book& book);
    BookPtr loadBookByID(BookID bookID);
    bool updateBook(BookID bookID, const std::string& name);
    bool removeBook(BookID bookID);

    std::optional<NoteID> insertNote(const Note& note);
    NotePtr loadNoteByID(NoteID noteID);
    bool updateNote(NoteID noteID, const std::string& content);
    bool removeNote(NoteID noteID);

    BooksCache loadBooks();
    NotesCache loadAllNotes();
    NotesCache loadNotesByBookID(BookID bookID);

private:
    bool hasBook(BookID bookID);
    bool hasNote(NoteID NoteID);

    const std::string _dbName;

    using DatabaseStorage = decltype(initStorage(""));
    DatabaseStorage _dbStorage;
};

}  // namespace core
