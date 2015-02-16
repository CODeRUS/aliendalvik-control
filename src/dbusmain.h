#ifndef DBUSMAIN_H
#define DBUSMAIN_H

#include <QObject>

class DBusMain : public QObject
{
    Q_OBJECT
public:
    explicit DBusMain(QObject *parent = 0);

private slots:
    void start();

};

#endif // DBUSMAIN_H
