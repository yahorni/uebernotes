#include <iostream>

#include "linux/argparser.hpp"
#include "linux/client.hpp"

auto parseArguments(int argc, char* argv[]) {
    try {
        linux::Argparser argparser;
        auto cli_args = argparser.parse(argc, argv);
        if (cli_args.count("help")) {
            std::cout << argparser.getHelp() << std::endl;
            exit(0);
        }
        return cli_args;
    } catch (const linux::ArgparserError& ex) {
        // TODO: print it properly, no need to log
        std::cout << ex.what() << std::endl;
        // TODO: check standard error codes
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    auto cli_args = parseArguments(argc, argv);

    if (cli_args.arguments().size() > 0)
        std::cout << "CLI mode" << std::endl;
    else
        std::cout << "TUI mode" << std::endl;

    linux::Client client;
    // auto& storage = client.getStorage();

    if (cli_args.count("list-books")) {
        client.listBooks();
    } else if (cli_args.count("print-book")) {
    } else if (cli_args.count("print-note")) {
    } else if (cli_args.count("create-book")) {
        client.createBook(cli_args["create-book"].as<std::string>());
    } else if (cli_args.count("create-note")) {
    }

    return 0;
}
