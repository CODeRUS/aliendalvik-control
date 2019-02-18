#ifndef WINDOWLAYOUT_H
#define WINDOWLAYOUT_H

#include "loggingclasswrapper.h"

class Parcel;
class WindowLayout : public LoggingClassWrapper
{
public:
    WindowLayout(Parcel *parcel, const char *loggingCategoryName = LOGGING(WindowLayout)".parcel");

    int width = -1;
    float widthFraction = -1;
    int height = -1;
    float heightFraction = -1;
    int gravity = -1;
    int minWidth = -1;
    int minHeight = -1;
};

#endif // WINDOWLAYOUT_H
