#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QObject>

class FileUtils : public QObject
{
    Q_OBJECT
public:
    explicit FileUtils(QObject *parent = 0);

    Q_INVOKABLE bool exists(const QString &path);

};

#endif // FILEUTILS_H
