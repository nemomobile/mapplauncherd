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


#ifndef CALCULATIONSTACK_H
#define CALCULATIONSTACK_H

#include <QStringList>
#include <QDebug>

class CalculationStack : public QStringList
{
public:
    static CalculationStack* instance();
    void push( const QString& value, const QString& op = "" );
    void generateCalculationCommand( QString& ) ;
private:
    CalculationStack() {}
    static CalculationStack* calculationStack;
};

#endif // CALCULATIONSTACK_H
