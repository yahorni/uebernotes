#include "linux/argparser.hpp"

#include "core/common_types.hpp"

namespace linux {

// TODO: add unittests for CmdLineArgs
// TODO: log all CmdLineError

CmdLineArgs::CmdLineArgs() {
    try {
        _options.add_options()                 //
            ("h,help", "Print help and exit")  //
            ("d,database", "Database file path", cxxopts::value<std::string>(), "<path>");
        _options.add_options("Operation")                                                             //
            ("l,list-books", "List all books")                                                        //
            ("print-book", "Print all notes from book", cxxopts::value<core::BookID>(), "<book_id>")  //
            ("print-note", "Print note", cxxopts::value<core::NoteID>(), "<note_id>")                 //
            ("b,create-book", "Create new book", cxxopts::value<std::string>(), "<name>")             //
            ("n,create-note", "Create new note", cxxopts::value<std::string>(), "<name>");
        _options.add_options("Note creation")                                                     //
            ("book-id", "Set book ID for new note", cxxopts::value<core::BookID>(), "<book_id>")  //
            ("content", "Set content for new note", cxxopts::value<std::string>(), "<string>");
    } catch (const cxxopts::OptionSpecException& ex) {
        throw CmdLineError(ex.what());
    }
}

void CmdLineArgs::parse(int argc, char* argv[]) {
    _parse(argc, argv);
    // TODO: separate validation if/when exclusive group will be implemented
    _validate();
}

bool CmdLineArgs::has(const std::string& arg) const { return _parseResult.count(arg) > 0; }

size_t CmdLineArgs::size() const { return _parseResult.arguments().size(); }

std::string CmdLineArgs::getString(const std::string& arg) const { return _parseResult[arg].as<std::string>(); }

uint64_t CmdLineArgs::getUInt64(const std::string& arg) const { return _parseResult[arg].as<uint64_t>(); }

void CmdLineArgs::_parse(int argc, char* argv[]) {
    try {
        _parseResult = _options.parse(argc, argv);
    } catch (const cxxopts::OptionParseException& ex) {
        throw CmdLineError(ex.what());
    }
}

void CmdLineArgs::_validate() {
    // TODO: make common dictionary/exclusive group subclass to use in ctor and here
    if (has("list-books") +       //
            has("print-book") +   //
            has("print-note") +   //
            has("create-book") +  //
            has("create-note") >
        1) {
        // TODO: print what operations are given
        throw CmdLineError("Several operations given");
    }

    if ((has("book-id") || has("content")) && !has("create-note")) {
        throw CmdLineError("Usage of note creation arguments without note name");
    }
}

std::string CmdLineArgs::help() const { return _options.help(); }

}  // namespace linux
