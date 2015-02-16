#include "adaptor.h"

#include <QCoreApplication>
#include <QProcess>
#include <QDebug>

Adaptor::Adaptor(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
}

Adaptor::~Adaptor()
{
}

void Adaptor::sendKeyevent(int code)
{
    QByteArray ANDROID_ROOT(qgetenv("ANDROID_ROOT"));
    qputenv("CLASSPATH", ANDROID_ROOT + "/framework/input.jar");

    QString program = ANDROID_ROOT + "/bin/app_process";
    QStringList arguments;
    arguments << (ANDROID_ROOT + "/bin") << "com.android.commands.input.Input" << "keyevent" << QString::number(code);

    QProcess myProcess;
    myProcess.startDetached(program, arguments);
}

