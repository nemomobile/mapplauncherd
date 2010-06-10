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

#ifndef CANVASPAGE_H
#define CANVASPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MButton>

class CanvasPage : public MApplicationPage
{
    Q_OBJECT
public:
    CanvasPage( const QString& title );
    virtual ~CanvasPage();

    virtual void createContent();

    void addPage( MApplicationPage* page );
public slots:
    void handleButtonClick();
private:
    QList<MApplicationPage*>  pages;
    MApplicationPage*         shownPage;
};

#endif // CANVASPAGE_H
