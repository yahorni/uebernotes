#include "linux/argparser.hpp"

#include <core/types.hpp>

#include <map>
#include <vector>

namespace linux {

// TODO: add unittests for CmdLineArgs
// TODO: maybe worth get rid of CmdLineError
// TODO: fix crash when passing "--v=2" -> "IOT instruction (core dumped)"

enum GroupIdx { standard = 0, operation = 1, create_update = 2 };
static const std::vector<std::string> groups{"", "Operation", "Create/update"};

enum class Op {
    list_books,
    print_book,
    print_note,
    create_book,
    create_note,
    update_book,
    update_note,
    remove_book,
    remove_note
};
static const std::map<Op, std::string> operations{
    {Op::list_books, "list-books"},   {Op::print_book, "print-book"},   {Op::print_note, "print-note"},
    {Op::create_book, "create-book"}, {Op::create_note, "create-note"}, {Op::update_book, "update-book"},
    {Op::update_note, "update-note"}, {Op::remove_book, "remove-book"}, {Op::remove_note, "remove-note"}};

CmdLineArgs::CmdLineArgs() {
    try {
        _options.add_options()                 //
            ("h,help", "Print help and exit")  //
            ("d,database", "Database file path", cxxopts::value<std::string>()->default_value("db.sqlite3"), "<path>");
        _options.add_options(groups.at(GroupIdx::operation))                                                       //
            (operations.at(Op::list_books), "List all books")                                                      //
            (operations.at(Op::print_book), "Print notes from book", cxxopts::value<core::BookID>(), "<book_id>")  //
            (operations.at(Op::print_note), "Print note", cxxopts::value<core::NoteID>(), "<note_id>")             //
            (operations.at(Op::create_book), "Create new book", cxxopts::value<std::string>(), "<name>")           //
            (operations.at(Op::create_note), "Create new note", cxxopts::value<core::BookID>(), "<book_id>")       //
            (operations.at(Op::update_book), "Update book", cxxopts::value<core::BookID>(), "<book_id>")           //
            (operations.at(Op::update_note), "Update note", cxxopts::value<core::BookID>(), "<note_id>")           //
            (operations.at(Op::remove_book), "Remove book", cxxopts::value<core::BookID>(), "<book_id>")           //
            (operations.at(Op::remove_note), "Remove note", cxxopts::value<core::BookID>(), "<note_id>");
        _options.add_options(groups.at(GroupIdx::create_update))                       //
            ("n,book-name", "Set book name", cxxopts::value<std::string>(), "<name>")  //
            ("c,note-content", "Set note content", cxxopts::value<std::string>(), "<string>");
    } catch (const cxxopts::exceptions::specification& ex) {
        throw CmdLineError(ex.what());
    }
}

std::string CmdLineArgs::help() const { return _options.help(groups); }

void CmdLineArgs::parse(int argc, const char* const* argv) {
    _parse(argc, argv);
    // TODO: separate validation if/when exclusive group will be implemented
    _validate();
}

bool CmdLineArgs::has(const std::string& arg) const { return _parseResult.count(arg) > 0; }

size_t CmdLineArgs::size() const { return _parseResult.arguments().size(); }

std::string CmdLineArgs::getString(const std::string& arg) const { return _parseResult[arg].as<std::string>(); }

uint64_t CmdLineArgs::getUInt64(const std::string& arg) const { return _parseResult[arg].as<uint64_t>(); }

bool CmdLineArgs::hasOperation() const {
    for (const auto& [_, op] : operations) {
        if (has(op)) {
            return true;
        }
    }
    return false;
}

void CmdLineArgs::_parse(int argc, const char* const* argv) {
    try {
        _parseResult = _options.parse(argc, argv);
    } catch (const cxxopts::exceptions::parsing& ex) {
        throw CmdLineError(ex.what());
    }
}

void CmdLineArgs::_validate() {
    // TODO: make common dictionary/exclusive group subclass to use in ctor and here
    uint8_t opsAmount = 0;
    for (const auto& [_, op] : operations) {
        if (has(op)) {
            opsAmount++;
        }
    }

    if (opsAmount > 1) {
        // TODO: print what operations are given
        throw CmdLineError("Several operations given");
    }
}

}  // namespace linux
