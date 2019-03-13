#ifndef SERVICE_H
#define SERVICE_H

#include <QObject>

class Service : public QObject
{
    Q_OBJECT
public:
    explicit Service(QObject *parent = nullptr);

public slots:
    void start();
};

#endif // SERVICE_H
