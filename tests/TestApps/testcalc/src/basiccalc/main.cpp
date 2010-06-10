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


#include <MApplication>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MLocale>
#include <MGConfItem>
#include <MApplicationService>
#include <MComponentCache>
#include <MExport>

#include <signal.h>

#include "basiccalc.h"

// LANGUAGE changes
#include "calcretranslator.h"

class BasicCalcApplicationService : public MApplicationService {
public:
    BasicCalcApplicationService( QObject* parent = 0 ) :
	    MApplicationService( DBUS_APPLICATION_NAME, parent ) {
    }
};


M_EXPORT int main( int argc, char** argv )
{
    MApplication* app = MComponentCache::mApplication(argc, argv, "testcalc", new BasicCalcApplicationService() );
    //MApplication* app = MComponentCache::mApplication(argc, argv, "essentials", new BasicCalcApplicationService() );
    //MApplication* app = MComponentCache::mApplication(argc, argv);
    qDebug() << "argc = " << argc << endl;
    qDebug() << "argv[0] = " << argv[0] << endl;
    
    MApplicationWindow* window = MComponentCache::mApplicationWindow();


#if 0
    CalcRetranslator calcRetranslator;
    QObject::connect( 
		&app, 
		SIGNAL(localeSettingsChanged()), 
		&calcRetranslator,
		SLOT(calcRetranslate()) );
#endif

    window->show();

    BasicCalc basicCalc;
    basicCalc.appear();
    
    return app->exec();
}


