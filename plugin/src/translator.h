#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QTranslator>
#include <QLocale>
#include <QGuiApplication>

class Translator : public QObject
{
    Q_OBJECT
public:
    explicit Translator(QObject *parent = 0);

private:
    QTranslator *translator;

public slots:
};

#endif // TRANSLATOR_H
