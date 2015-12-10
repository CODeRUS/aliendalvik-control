#include "translator.h"
#include <QDebug>

Translator::Translator(QObject *parent) : QObject(parent)
{
    translator = new QTranslator;
    translator->load(QLocale::system(), "powermenu", "_", "/usr/share/powermenu2/translations");
    QGuiApplication::instance()->installTranslator(translator);

}

