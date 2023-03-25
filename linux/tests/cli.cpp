#include <catch2/catch_test_macros.hpp>

#include "core/appcontext.hpp"
#include "linux/argparser.hpp"
#include "linux/cli.hpp"
#include "linux/metadata.hpp"

#include <filesystem>
#include <iostream>
#include <string>

core::BookID createBook(core::Storage& storage, std::string name) {
    core::BookInfo bookInfo{std::move(name)};
    auto bookID = storage.createBook(std::move(bookInfo));
    REQUIRE(bookID.has_value());
    return bookID.value();
}

TEST_CASE("cli", "[linux]") {
    namespace fs = std::filesystem;
    const auto database = "testdb-linux.sqlite3";

    // remove old database
    if (fs::exists(database)) REQUIRE(fs::remove(database));

    core::AppContext context{database, false};
    core::Storage storage{context};

    SECTION("note creation") {
        // prepare book in db
        auto bookID = createBook(storage, "book_for_note");
        auto bookIDStr = std::to_string(bookID);

        // prepare note in cli
        const auto noteContent = "some large string with note 1 content to run the test";

        // prepare env args
        int argc = 7;
        const char* const argv[]{linux::program_name,                     //
                                 "--create-note",     bookIDStr.c_str(),  //
                                 "--note-content",    noteContent,        //
                                 "--database",        database};

        std::cout << argc << argv << std::endl;

        // run client
        linux::CmdLineArgs cliArgs;
        cliArgs.parse(argc, argv);
        auto cli = linux::CLI{context};
        cli.run(cliArgs);

        // // check db state
        auto loadedBook = storage.getBook(bookID);
        auto loadedNotes = loadedBook->getNotesInfo();
        REQUIRE(loadedNotes.size() == 1);
        auto loadedNote = *loadedNotes.begin();
        REQUIRE(loadedNote->content == noteContent);
    }

    SECTION("list_books") {
        // prepare book in db
        createBook(storage, "book1");
        createBook(storage, "book2");
        createBook(storage, "book3");
        createBook(storage, "book4");

        // prepare env args
        int argc = 4;
        const char* const argv[]{linux::program_name,  //
                                 "--list-books",       //
                                 "--database", database};

        // run client
        linux::CmdLineArgs cliArgs;
        cliArgs.parse(argc, argv);
        auto cli = linux::CLI{context};
        cli.run(cliArgs);

        // check db state
        auto loadedBooks = storage.getBooksInfo();
        REQUIRE(loadedBooks.size() == 4);
    }
}
