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

#include "calculationstack.h"

CalculationStack* CalculationStack::calculationStack= NULL;

CalculationStack* CalculationStack::instance()
{
    if( calculationStack == NULL )
        calculationStack = new CalculationStack();

    return calculationStack;
}

void CalculationStack::push( const QString& value, const QString& op )
{
    bool embedBrackets = false;
    
    if( value.at(0) == '-' )
    {
        embedBrackets = true;
    }
    
    if( embedBrackets == true )
	calculationStack->push_back( "(" );
    calculationStack->push_back( value );
    if( embedBrackets == true )
	calculationStack->push_back( ")" );
    
    if( op!= "" )
        calculationStack->push_back( op );

    for( CalculationStack::const_iterator i = calculationStack->constBegin();
	 i != calculationStack->constEnd();
	 i++ )
	qDebug() << *i << endl;
}

void CalculationStack::generateCalculationCommand( QString& calculationCommand )
{
    for( CalculationStack::const_iterator i = calculationStack->constBegin();
         i != calculationStack->constEnd();
         i++ )
        calculationCommand.append(*i);
}

