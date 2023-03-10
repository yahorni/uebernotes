#include "linux/cli.hpp"

#include <iostream>

#include "core/config.hpp"
#include "linux/client.hpp"

namespace linux {

CLI::CLI(const CmdLineArgs& args) {
    core::Config cfg;
    if (args.has("database")) cfg.database = args.getString("database");

    linux::Client client(cfg);

    // TODO: add operation enum
    if (args.has("list-books")) {
        client.listBooks();
    } else if (args.has("print-book")) {
    } else if (args.has("print-note")) {
    } else if (args.has("create-book")) {
        client.createBook(args.getString("create-book"));
    } else if (args.has("create-note")) {
    }
}

}  // namespace linux
