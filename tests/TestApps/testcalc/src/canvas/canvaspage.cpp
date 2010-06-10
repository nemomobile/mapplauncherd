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


#include "canvaspage.h"

CanvasPage::CanvasPage( const QString& title ) :
                MApplicationPage(), shownPage(NULL)
{
    setTitle( title );
}

CanvasPage::~CanvasPage()
{
}

void CanvasPage::addPage( MApplicationPage* page )
{
    page->setParent( this );
    pages.append( page );
}

void CanvasPage::createContent()
{
    MApplicationPage::createContent();

    this->setObjectName( "calcpage" );

    MWidget* panel = this->centralWidget();
    MLayout* layout = new MLayout( panel );
    MLinearLayoutPolicy* policy = new MLinearLayoutPolicy( layout, Qt::Vertical );
    policy->setContentsMargins( 12, 12, 12, 0 );
    policy->setSpacing(12);

    int index = 0;
    foreach( MApplicationPage* page, pages )
    {
        MButton* basicCalcButton = new MButton( page->title(), this->centralWidget() );
        policy->addItem( basicCalcButton );
        connect(
                basicCalcButton,
                SIGNAL(clicked()),
                this,
                SLOT(handleButtonClick()) );

        index++;
    }

    centralWidget()->setContentsMargins( 0, 0, 0, 25 );
}

void CanvasPage::handleButtonClick()
{
    QObject* senderObject = sender();
    if( senderObject )
    {
        MButton* button = qobject_cast<MButton*>( senderObject );
        if( button )
        {
            foreach( MApplicationPage* page, pages )
                if( page->title() == button->text() ) 
                {
                    page->appearNow();
                    shownPage = page;
                }
        }
    }
}
