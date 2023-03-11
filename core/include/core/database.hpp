#pragma once

#include "core/book.hpp"
#include "core/note.hpp"

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
                        sql::make_column("id", &BookInfo::id, sql::primary_key().autoincrement()),  //
                        sql::make_column("name", &BookInfo::name)),                                 //
        sql::make_table("normal_notes",                                                             //
                        sql::make_column("id", &NoteInfo::id, sql::primary_key().autoincrement()),  //
                        sql::make_column("book_id", &NoteInfo::bookID),                             //
                        sql::make_column("name", &NoteInfo::name),                                  //
                        sql::make_column("content", &NoteInfo::content),                            //
                        sql::foreign_key(&NoteInfo::bookID).references(&BookInfo::id)));
}

class Database {
public:
    explicit Database(std::string_view dbName);

    // TODO: should I really restrict it explicitly (what about moving?) -- yes
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    BookID storeBook(const BookInfo& book);
    NoteID storeNote(const NoteInfo& note);

    std::optional<BookInfo> loadBookByID(BookID bookID);
    std::optional<NoteInfo> loadNoteByID(NoteID noteID);

    BooksInfoCollection loadBooks();
    NotesInfoCollection loadNotesByBookID(BookID bookID);

private:
    using DatabaseStorage = decltype(initStorage(""));

    const std::string _dbName;
    DatabaseStorage _dbStorage;
};

}  // namespace core
