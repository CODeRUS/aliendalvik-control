#include "binderinterfaceabstract.h"
#include "parcel.h"
#include "windowlayout.h"

WindowLayout::WindowLayout(Parcel *parcel ,const char *loggingCategoryName)
    : LoggingClassWrapper(loggingCategoryName)
{
    width = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "width:" << width;
    widthFraction = parcel->readFloat();
    qCDebug(logging) << Q_FUNC_INFO << "widthFraction:" << widthFraction;
    height = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "height:" << height;
    heightFraction = parcel->readFloat();
    qCDebug(logging) << Q_FUNC_INFO << "heightFraction:" << heightFraction;
    gravity = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "gravity:" << gravity;
    minWidth = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "minWidth:" << minWidth;
    minHeight = parcel->readInt();
    qCDebug(logging) << Q_FUNC_INFO << "minHeight:" << minHeight;
}
