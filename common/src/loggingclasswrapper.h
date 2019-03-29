#ifndef LOGGINGCLASSWRAPPER_H
#define LOGGINGCLASSWRAPPER_H

#include <QLoggingCategory>

#define LOGGING(x) "logging"#x

class LoggingClassWrapper
{
public:
    LoggingClassWrapper(const char *category, QtMsgType type = QtWarningMsg);

    QLoggingCategory logging;
};

#endif // LOGGINGCLASSWRAPPER_H
