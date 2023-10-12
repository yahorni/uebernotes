#include "core/logger.hpp"

namespace core {

Log::CallbackType Log::_logTrace = Log::_suppressLogs;
Log::CallbackType Log::_logDebug = Log::_suppressLogs;
Log::CallbackType Log::_logInfo = Log::_suppressLogs;
Log::CallbackType Log::_logWarning = Log::_suppressLogs;
Log::CallbackType Log::_logError = Log::_suppressLogs;
Log::CallbackType Log::_logFatal = Log::_suppressLogs;

}  // namespace core
