/* 
 * This file is part of calculator application 
 *
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Abhijit Apte <abhijit.apte@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */


#ifndef COREINTERFACE_H
#define COREINTERFACE_H

#include <QObject>

#include "calculationstack.h"

class CoreInterface : public QObject
{
    Q_OBJECT
public:
    static CoreInterface* instance();
    void calculate();
    static CalculationStack* calculationStack();

private:
    CoreInterface();
    static CoreInterface* core;

    static const QString program;
    static CalculationStack* stack;

signals:
    void result( const QString& );
};


#endif // COREINTERFACE_H
