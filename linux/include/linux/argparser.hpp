#pragma once

#include <cxxopts.hpp>

namespace linux {

class ArgparserError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class Argparser {
public:
    Argparser();
    cxxopts::ParseResult parse(int argc, char* argv[]);
    std::string getHelp() const;

private:
    cxxopts::ParseResult _parse(int argc, char* argv[]);
    void _validate(const cxxopts::ParseResult& result);


    cxxopts::Options _options{"uebernotes-cli", "Note-taking app for specific purposes"};
};

} // namespace linux
