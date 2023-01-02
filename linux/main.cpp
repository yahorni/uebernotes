#include <iostream>

#include "core/config.hpp"

#include "linux/argparser.hpp"
#include "linux/client.hpp"
#include "linux/tui.hpp"

auto parseArguments(int argc, char* argv[]) {
    try {
        linux::CmdLineArgs clArgs;
        clArgs.parse(argc, argv);
        if (clArgs.has("help")) {
            std::cout << clArgs.help() << std::endl;
            exit(0);
        }
        return clArgs;
    } catch (const linux::CmdLineError& ex) {
        // TODO: print it properly, no need to log
        std::cout << ex.what() << std::endl;
        // TODO: check standard error codes
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    auto clArgs = parseArguments(argc, argv);

    if (clArgs.size() > 0) {
        std::cout << "CLI mode" << std::endl;
    } else {
        std::cout << "TUI mode" << std::endl;
        linux::TUI tui;
        exit(0);
    }

    core::Config cfg;
    if (clArgs.has("database")) cfg.database = clArgs.getString("database");

    linux::Client client(cfg);

    // TODO: add operation enum
    if (clArgs.has("list-books")) {
        client.listBooks();
    } else if (clArgs.has("print-book")) {
    } else if (clArgs.has("print-note")) {
    } else if (clArgs.has("create-book")) {
        client.createBook(clArgs.getString("create-book"));
    } else if (clArgs.has("create-note")) {
    }

    return 0;
}
