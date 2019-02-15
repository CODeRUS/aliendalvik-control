#include "binderinterfaceabstract.h"
#include "windowlayout.h"

#include <QDebug>

WindowLayout::WindowLayout(Parcel *parcel)
{
    width = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "width:" << width;
    widthFraction = parcel->readFloat();
    qDebug() << Q_FUNC_INFO << "widthFraction:" << widthFraction;
    height = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "height:" << height;
    heightFraction = parcel->readFloat();
    qDebug() << Q_FUNC_INFO << "heightFraction:" << heightFraction;
    gravity = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "gravity:" << gravity;
    minWidth = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "minWidth:" << minWidth;
    minHeight = parcel->readInt();
    qDebug() << Q_FUNC_INFO << "minHeight:" << minHeight;
}
