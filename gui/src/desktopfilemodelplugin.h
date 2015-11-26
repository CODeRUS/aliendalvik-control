#ifndef DESKTOPFILEMODELPLUGIN_H
#define DESKTOPFILEMODELPLUGIN_H

#include <QQmlExtensionPlugin>

class DesktopFileModelPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // DESKTOPFILEMODELPLUGIN_H
