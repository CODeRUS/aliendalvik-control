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
    runCommand("input.jar", QStringList() << "com.android.commands.input.Input" << "keyevent" << QString::number(code));
}

void Adaptor::sendInput(const QString &text)
{
    runCommand("input.jar", QStringList() << "com.android.commands.input.Input" << "text" << text);
}

void Adaptor::sendIntent(const QString &intent)
{
    runCommand("am.jar", QStringList() << "com.android.commands.am.Am" << "broadcast" << "-a" << intent);
}

void Adaptor::runCommand(const QString &jar, const QStringList &params)
{
    QString ANDROID_ROOT(qgetenv("ANDROID_ROOT"));
    qputenv("CLASSPATH", QString("%1/framework/%2").arg(ANDROID_ROOT).arg(jar).toUtf8());

    QString program = ANDROID_ROOT + "/bin/app_process";
    QStringList arguments;
    arguments << (ANDROID_ROOT + "/bin") << params;

    QProcess myProcess;
    myProcess.startDetached(program, arguments);
}

