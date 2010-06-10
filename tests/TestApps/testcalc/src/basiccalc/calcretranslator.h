#ifndef CALCRETRANSLATOR_H
#define CALCRETRANSLATOR_H

#include <QObject>

class CalcRetranslator : public QObject
{
    Q_OBJECT
public slots:
    void calcRetranslate();
};

#endif

