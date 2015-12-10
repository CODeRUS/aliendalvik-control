#include "fileutils.h"
#include <QFile>

FileUtils::FileUtils(QObject *parent) : QObject(parent)
{

}

bool FileUtils::exists(const QString &path)
{
    return QFile(path).exists();
}

