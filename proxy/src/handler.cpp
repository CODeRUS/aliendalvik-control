#include "handler.h"
#include "aliendalvikcontrol_interface.h"

#include <QDebug>

Handler::Handler(DBusInterface *iface, QObject *parent)
    : QObject(parent)
    , m_iface(iface)
{

}

void Handler::open(const QStringList &params)
{
    qDebug() << Q_FUNC_INFO << params;

    m_iface->uriActivity(params.first());
}

void Handler::openSelector(const QStringList &params)
{
    qDebug() << Q_FUNC_INFO << params;

    m_iface->uriActivitySelector(params.first());
}
