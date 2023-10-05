#include "core/appcontext.hpp"
#include "linux/argparser.hpp"
#include "linux/cli.hpp"
#include "linux/logger.hpp"
#include "linux/tui.hpp"

#include <iostream>

using linux::Log;

LOGGER_STATIC_INIT

auto parseArguments(int argc, const char* argv[]) {
    try {
        linux::CmdLineArgs clArgs;
        clArgs.parse(argc, argv);
        if (clArgs.has("help")) {
            std::cout << clArgs.help() << std::endl;
            std::exit(0);
        }
        return clArgs;
    } catch (const linux::CmdLineError& ex) {
        Log::error("Command line error: {}", ex.what());
        std::cerr << "Command line error: " << ex.what() << std::endl;
        // TODO: check standard error codes
        std::exit(1);
    }
}

int main(int argc, const char* argv[]) {
    Log::init(argc, argv);
    auto cliArgs = parseArguments(argc, argv);

    const core::AppContext context{cliArgs.getString("database"), !cliArgs.hasOperation()};

    Log::info("Using database: {}", context.database.c_str());
    Log::info("Using cache: {}", context.useCaching);

    if (cliArgs.hasOperation() || cliArgs.has("help")) {
        Log::debug("CLI mode");
        linux::CLI cli{context};
        return cli.run(cliArgs) ? 0 : 1;
    } else {
        Log::debug("TUI mode");
        linux::TUI tui{context};
        return tui.run() ? 0 : 1;
    }
}
