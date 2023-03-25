#include "linux/cli.hpp"

#include "core/appcontext.hpp"

namespace linux {

CLI::CLI(const core::AppContext& context)
    : _client(context) {}

bool CLI::run(const CmdLineArgs& args) {
    // TODO: add operation enum
    if (args.has("list-books")) {
        return _client.listBooks();
    } else if (args.has("print-book")) {
        return _client.printBook(args.getUInt64("print-book"));
    } else if (args.has("print-note")) {
        return _client.printNote(args.getUInt64("print-note"));
    } else if (args.has("create-book")) {
        return _client.createBook(args.getString("create-book"));
    } else if (args.has("create-note")) {
        return _client.createNote(args.getUInt64("create-note"), args.getString("note-content"));
    } else if (args.has("update-book")) {
        return _client.updateBook(args.getUInt64("update-book"), args.getString("book-name"));
    } else if (args.has("update-note")) {
        return _client.updateNote(args.getUInt64("update-note"), args.getString("note-content"));
    } else if (args.has("remove-book")) {
        return _client.removeBook(args.getUInt64("remove-book"));
    } else if (args.has("remove-note")) {
        return _client.removeNote(args.getUInt64("remove-note"));
    }

    return false;
}

}  // namespace linux
