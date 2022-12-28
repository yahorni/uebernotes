#pragma once

#include <memory>
#include <string_view>

#include <sqlite_orm/sqlite_orm.h>
// https://github.com/fnc12/sqlite_orm/blob/master/examples/private_class_members.cpp

#include "book.hpp"
#include "note.hpp"

namespace uebernotes {

namespace sql = sqlite_orm;

inline auto initStorage(std::string_view _dbName) {
    return sql::make_storage(                                                                      //
        std::string(_dbName),                                                                      //
        sql::make_table("normal_books",                                                            //
                        sql::make_column("id", &BookInfo::id, sql::primary_key().autoincrement()), //
                        sql::make_column("name", &BookInfo::name)),                                //
        sql::make_table("normal_notes",                                                            //
                        sql::make_column("id", &NoteInfo::id, sql::primary_key().autoincrement()), //
                        sql::make_column("book_id", &NoteInfo::bookID),                            //
                        sql::make_column("name", &NoteInfo::name),                                 //
                        sql::make_column("content", &NoteInfo::content),                           //
                        sql::foreign_key(&NoteInfo::bookID).references(&BookInfo::id)));
}

class Database {
public:
    Database();
    explicit Database(std::string_view dbName);
    Database(const Database&) = delete;
    // TODO: restrict copying (and moving?)

    BookID storeBook(const BookInfo& book);
    NoteID storeNote(const NoteInfo& note);

    BookInfo getBookInfoByID(BookID bookID);
    NoteInfo getNoteInfoByID(NoteID noteID);

    NotesInfoCollection getNotesByBookID(BookID bookID);

private:
    using DatabaseStorage = decltype(initStorage(""));

    const std::string _dbName;
    DatabaseStorage _dbStorage;

    static const char _defaultDatabaseName[];
};

} // namespace uebernotes
