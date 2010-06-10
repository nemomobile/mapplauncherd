#ifndef CALCUTILITIES_H
#define CALCUTILITIES_H

#include <QObject>
#include <QTimer>
#include <MInfoBanner>
#include <QDebug>

#include <iostream>
#include <sstream>

using namespace std;

class CalcUtilities : public QObject
{
    Q_OBJECT
public:
    static CalcUtilities* instance();
    void showNotificationBanner( const QString& banner, const QString& icon );
    void convertToExponential( QString& numeral );
public slots:
    void notificationTimeout();
private:
    CalcUtilities();
    static CalcUtilities* utility;
};

#endif

