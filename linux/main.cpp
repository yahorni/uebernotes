#include "core/appcontext.hpp"
#include "linux/argparser.hpp"
#include "linux/cli.hpp"
#include "linux/tui.hpp"

#include <iostream>

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
    auto cliArgs = parseArguments(argc, argv);
    const core::AppContext context{cliArgs.getString("database"), !cliArgs.hasOperation()};

    if (cliArgs.hasOperation() || cliArgs.has("help")) {
        std::cout << "CLI mode" << std::endl;
        linux::CLI cli{context};
        cli.run(cliArgs);
    } else {
        std::cout << "TUI mode" << std::endl;
        linux::TUI tui{context};
        tui.run();
    }

    return 0;
}
