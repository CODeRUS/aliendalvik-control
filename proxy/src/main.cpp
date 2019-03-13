#include "service.h"

#include <QCoreApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Service service(&app);
    QTimer::singleShot(0, &service, &Service::start);
    return app.exec();
}
