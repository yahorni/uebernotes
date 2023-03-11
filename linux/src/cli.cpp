#include "linux/cli.hpp"

namespace linux {

CLI::CLI(const core::Config& config)
    : _client(config) {}

void CLI::run(const CmdLineArgs& args) {
    // TODO: add operation enum
    if (args.has("list-books")) {
        _client.listBooks();
    } else if (args.has("print-book")) {
    } else if (args.has("print-note")) {
    } else if (args.has("create-book")) {
        _client.createBook(args.getString("create-book"));
    } else if (args.has("create-note")) {
        _client.createNote(args.getUInt64("book-id"), args.getString("create-note"), args.getString("content"));
    }
}

}  // namespace linux
