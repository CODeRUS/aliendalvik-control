#ifndef POWERMENUPLUGIN_H
#define POWERMENUPLUGIN_H

#include <QQmlExtensionPlugin>

class PowermenuPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // POWERMENUPLUGIN_H
