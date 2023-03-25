#pragma once

#include "core/book.hpp"
#include "core/note.hpp"
#include "core/types.hpp"

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
                        sql::make_column("id", &BookInfo::id, sql::primary_key().autoincrement()),  //
                        sql::make_column("name", &BookInfo::name)),                                 //
        sql::make_table("normal_notes",                                                             //
                        sql::make_column("id", &NoteInfo::id, sql::primary_key().autoincrement()),  //
                        sql::make_column("book_id", &NoteInfo::bookID),                             //
                        sql::make_column("content", &NoteInfo::content),                            //
                        sql::foreign_key(&NoteInfo::bookID).references(&BookInfo::id)));
}

class Database {
public:
    explicit Database(std::string_view dbName);

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;

    std::optional<BookID> insertBook(const BookInfo& book);
    std::optional<NoteID> insertNote(const NoteInfo& note);

    std::shared_ptr<BookInfo> loadBookByID(BookID bookID);
    std::shared_ptr<NoteInfo> loadNoteByID(NoteID noteID);
    BooksCache loadBooks();
    NotesCache loadAllNotes();
    NotesCache loadNotesByBookID(BookID bookID);

    bool updateBook(BookID bookID, const std::string& name);
    bool updateNote(NoteID noteID, const std::string& content);

private:
    using DatabaseStorage = decltype(initStorage(""));

    const std::string _dbName;
    DatabaseStorage _dbStorage;
};

}  // namespace core
