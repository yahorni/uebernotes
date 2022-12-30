#include "linux/argparser.hpp"

#include "core/common_types.hpp"

namespace linux {

// TODO: add unittests for Argparser
// TODO: log all ArgparserError

Argparser::Argparser() {
    try {
        _options.add_options()                //
            ("h,help", "Print help and exit") //
                                              // NOTE: database ignored till configuration not implemented
            ("d,database", "Database file path", cxxopts::value<std::string>(), "<path>");
        _options.add_options("Operation")                                                                  //
            ("l,list-books", "List all books")                                                             //
            ("print-book", "Print all notes from book", cxxopts::value<uebernotes::BookID>(), "<book_id>") //
            ("print-note", "Print note", cxxopts::value<uebernotes::NoteID>(), "<note_id>")                //
            ("b,create-book", "Create new book", cxxopts::value<std::string>(), "<name>")                  //
            ("n,create-note", "Create new note", cxxopts::value<std::string>(), "<name>");
        _options.add_options("Note creation")                                                          //
            ("book-id", "Set book ID for new note", cxxopts::value<uebernotes::BookID>(), "<book_id>") //
            ("content", "Set content for new note", cxxopts::value<std::string>(), "<string>");
    } catch (const cxxopts::OptionSpecException& ex) {
        throw ArgparserError(ex.what());
    }
}

cxxopts::ParseResult Argparser::parse(int argc, char* argv[]) {
    auto result = _parse(argc, argv);
    _validate(result);
    return result;
}

cxxopts::ParseResult Argparser::_parse(int argc, char* argv[]) {
    try {
        return _options.parse(argc, argv);
    } catch (const cxxopts::OptionParseException& ex) {
        throw ArgparserError(ex.what());
    }
}

void Argparser::_validate(const cxxopts::ParseResult& result) {
    // TODO: make common dictionary to use in ctor and here
    if ((result.count("list-books") > 0) +      //
            (result.count("print-book") > 0) +  //
            (result.count("print-note") > 0) +  //
            (result.count("create-book") > 0) + //
            (result.count("create-note") > 0) >
        1) {
        // TODO: print what operations are given
        throw ArgparserError("Several operations given");
    }

    if ((result.count("book-id") || result.count("content")) && !result.count("create-note")) {
        throw ArgparserError("Usage of note creation arguments without note name");
    }
}

std::string Argparser::getHelp() const { return _options.help(); }

} // namespace linux
