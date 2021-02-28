#include "logging.hpp"

ModInfo myLoggerInfo = {ID, VERSION};

Logger& getLogger()
{
    static Logger* logger = new Logger(myLoggerInfo, LoggerOptions(false, true));
    return *logger;
}