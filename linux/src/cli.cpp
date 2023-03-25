#include "linux/cli.hpp"

#include "core/appcontext.hpp"

namespace linux {

CLI::CLI(const core::AppContext& context)
    : _client(context) {}

void CLI::run(const CmdLineArgs& args) {
    // TODO: add operation enum
    if (args.has("list-books")) {
        _client.listBooks();
    } else if (args.has("print-book")) {
        _client.printBook(args.getUInt64("print-book"));
    } else if (args.has("print-note")) {
        _client.printNote(args.getUInt64("print-note"));
    } else if (args.has("create-book")) {
        _client.createBook(args.getString("create-book"));
    } else if (args.has("create-note")) {
        _client.createNote(args.getUInt64("create-note"), args.getString("note-content"));
    } else if (args.has("update-book")) {
        _client.updateBook(args.getUInt64("update-book"), args.getString("book-name"));
    } else if (args.has("update-note")) {
        _client.updateNote(args.getUInt64("update-note"), args.getString("note-content"));
    }
}

}  // namespace linux
