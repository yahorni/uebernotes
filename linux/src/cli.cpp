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
        _client.createNote(args.getUInt64("book-id"), args.getString("create-note"), args.getString("content"));
    }
}

}  // namespace linux
