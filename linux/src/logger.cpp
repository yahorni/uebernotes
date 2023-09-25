#include "linux/logger.hpp"

#include "core/logger.hpp"

namespace linux {

#define LOG_CORE(NAME, LEVEL)                                                                                          \
    void Log::core##NAME(const char* msg) { CLOG(LEVEL, "core") << msg; }

LOG_CORE(Debug, DEBUG)
LOG_CORE(Info, INFO)
LOG_CORE(Warning, WARNING)
LOG_CORE(Error, ERROR)
LOG_CORE(Fatal, FATAL)

// TODO: setup options parsing and showing in help message
// Options:
// -v/--verbose
// --v=2
// -vmodule=MODULE_NAME
// --logging-flags=3
// --default-log-file=FILE

void Log::init(int argc, const char** argv) {
    START_EASYLOGGINGPP(argc, argv);

    // TODO: move configuration to file
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    // log message
    defaultConf.setGlobally(el::ConfigurationType::Format, "%datetime %level [%logger] %msg");
    // log file
    // TODO: configurable path
    defaultConf.setGlobally(el::ConfigurationType::Filename, "uebernotes.log");
    // log rotate 2MB
    defaultConf.setGlobally(el::ConfigurationType::MaxLogFileSize, "2097152");
    // stdout/stderr
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    // configure all loggers
    el::Loggers::setDefaultConfigurations(defaultConf, true);

    // register loggers
    el::Loggers::getLogger(_linuxLoggerName);
    el::Loggers::getLogger(_coreLoggerName);

    core::Log::setDebugLogger(&Log::coreDebug);
    core::Log::setInfoLogger(&Log::coreInfo);
    core::Log::setWarningLogger(&Log::coreWarning);
    core::Log::setErrorLogger(&Log::coreError);
    core::Log::setFatalLogger(&Log::coreFatal);
}

}  // namespace linux
