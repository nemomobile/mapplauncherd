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

#include "calcconstants.h"
#include "coreinterface.h"

#include <QDebug>

CoreInterface* CoreInterface::core = NULL;
const QString CoreInterface::program = CALC_ENGINE_PROGRAM;
CalculationStack* CoreInterface::stack = NULL;


CoreInterface* CoreInterface::instance()
{
    if( core == NULL )
    {
        core = new CoreInterface();
	stack = CalculationStack::instance();
    }

    return core;
}

CoreInterface::CoreInterface()
{
}

void CoreInterface::calculate()
{
    QString calculationLineValue("777");
    emit result( calculationLineValue );
}

#if 0
void CoreInterface::calculate()
{	
    QString calculationLineValue;

    QString cmd;
    stack->generateCalculationCommand( cmd );
    
    QString command = this->program + " -d '" + cmd + "'" ;

    qDebug() << "final command to engine is : " << endl;
    qDebug() << command.toLatin1().data() << endl;

    FILE* fp = popen( command.toLatin1().data(), "r" );
    if( fp )
    {
        char buff[320];
        if( fgets( buff, 320, fp ) != NULL )
        {
            pclose( fp );
            calculationLineValue = buff;
        }
	else
	    pclose( fp );
    }

    calculationLineValue = calculationLineValue.simplified();

    emit result( calculationLineValue );
}
#endif

CalculationStack* CoreInterface::calculationStack()
{
    if( stack == NULL )
	// something got missed and somehow stack became NULL
	// try obtaining the singleton instance again
	stack = CalculationStack::instance();

    return stack;
}

