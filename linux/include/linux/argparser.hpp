#pragma once

#include "linux/metadata.hpp"

#include <string>
#include <cxxopts.hpp>

namespace linux {

class CmdLineError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class CmdLineArgs {
public:
    CmdLineArgs();
    void parse(int argc, const char* const* argv);

    std::string help() const;
    bool has(const std::string& arg) const;
    size_t size() const;

    bool hasOperation() const;

    std::string getString(const std::string& arg) const;
    uint64_t getUInt64(const std::string& arg) const;

private:
    void _parse(int argc, const char* const* argv);
    void _validate();

    cxxopts::Options _options{program_name, program_description};
    cxxopts::ParseResult _parseResult;
};

}  // namespace linux
