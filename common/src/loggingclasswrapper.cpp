#include "loggingclasswrapper.h"

LoggingClassWrapper::LoggingClassWrapper(const char *category, QtMsgType type)
    : logging(category, type)
{

}
