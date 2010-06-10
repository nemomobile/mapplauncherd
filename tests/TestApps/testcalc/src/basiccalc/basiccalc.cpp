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


#include "basiccalc.h"

#include <MWidgetCreator>
#include <MApplicationService>

M_REGISTER_WIDGET( BasicCalc )

BasicCalc::BasicCalc()
                : calculationStarted( false ), 
		  calcState( START ), 
		  exceedingLimitBannerDisplayed( false ), 
		  operation( "" ), 
		  saveText( NULL ), 
		  saveDialog( NULL ),
		  saveCalculationFromHistory( false ),
		  calculationDone( false ),
		  saveInProgress( false )
{
    setTitle( TXT_CALC_TITLE );
    // use the singleton instance
    core = CoreInterface::instance();
    utility = CalcUtilities::instance();

    MLocale locale;
    qDebug() << locale.formatNumber(0) << endl;
    qDebug() << locale.formatNumber(1) << endl;
    qDebug() << locale.formatNumber(2) << endl;
    qDebug() << locale.formatNumber(3) << endl;
    qDebug() << locale.formatNumber(4) << endl;
    qDebug() << locale.formatNumber(5) << endl;
    qDebug() << locale.formatNumber(6) << endl;
    qDebug() << locale.formatNumber(7) << endl;
    qDebug() << locale.formatNumber(8) << endl;
    qDebug() << locale.formatNumber(9) << endl;
    
    numeralMap[locale.formatNumber(0)==""? "0":locale.formatNumber(0)] = "0";
    numeralMap[locale.formatNumber(1)==""? "1":locale.formatNumber(1)] = "1";
    numeralMap[locale.formatNumber(2)==""? "2":locale.formatNumber(2)] = "2";
    numeralMap[locale.formatNumber(3)==""? "3":locale.formatNumber(3)] = "3";
    numeralMap[locale.formatNumber(4)==""? "4":locale.formatNumber(4)] = "4";
    numeralMap[locale.formatNumber(5)==""? "5":locale.formatNumber(5)] = "5";
    numeralMap[locale.formatNumber(6)==""? "6":locale.formatNumber(6)] = "6";
    numeralMap[locale.formatNumber(7)==""? "7":locale.formatNumber(7)] = "7";
    numeralMap[locale.formatNumber(8)==""? "8":locale.formatNumber(8)] = "8";
    numeralMap[locale.formatNumber(9)==""? "9":locale.formatNumber(9)] = "9";
    
    operationMap[QString(TXT_CALC_CLEAR)== "!! "? "C" : TXT_CALC_CLEAR] = "C";
    operationMap[QString(TXT_CALC_BACK)== "!! "? "<-" : TXT_CALC_BACK] = "<-";
    operationMap[QString(TXT_CALC_SIGN)== "!! "? "+/-" : TXT_CALC_SIGN] = "+/-";
    operationMap[locale.formatNumber(1.2)==""? ".":QString(locale.formatNumber(1.2)[1])] = ".";
    operationMap[QString(TXT_CALC_DIVIDE)== "!! "? "/" : TXT_CALC_DIVIDE ] = "/";
    operationMap[TXT_CALC_MULTIPLY] = "*";
    operationMap[QString(TXT_CALC_SUBTRACT)== "!! "? "-" : TXT_CALC_SUBTRACT] = "-";
    operationMap[QString(TXT_CALC_ADD)== "!! "? "+" : TXT_CALC_ADD] = "+";
    operationMap[QString(TXT_CALC_EQUAL)== "!! "? "=" : TXT_CALC_EQUAL] = "=";
}

BasicCalc::~BasicCalc()
{
    if( !storedReference.isEmpty() )
        saveCalculationTo( HISTORY );
    delete calculationHistory;
    delete savedCalculations;
}

void BasicCalc::createContent()
{
    this->setEscapeMode(MApplicationPageModel::EscapeCloseWindow);
    
    this->setObjectName( "calcpage" );
    this->setPannable( false );
    
    // Take the central widget of the page
    panel = this->centralWidget();
    
    MAction* action;
    
    action = new MAction( TXT_CALC_COPY, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), this, SLOT( copy() ) );
    
    action = new MAction( TXT_CALC_PASTE, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), this, SLOT( paste() ) );
    
    action = new MAction(TXT_CALC_SAVE_ME, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect(
            action,
            SIGNAL(triggered(bool)),
            this,
            SLOT(showSaveCalcDialog()));
    
    action = new MAction(TXT_CALC_HISTORY, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );

    calculationHistory = new CalculationHistory();
    
    connect(
	    calculationHistory,
	    SIGNAL( itemClicked(const QString&) ),
	    this,
	    SLOT( calculationHistoryItemClicked(const QString&) ) );

    connect(
	    calculationHistory,
	    SIGNAL( save(CalculationItem&) ),
	    this,
	    SLOT( saveFromHistory(CalculationItem&) ) );
    
    connect( action, SIGNAL( triggered(bool) ), this, SLOT(showCalculationHistory()) );

    action = new MAction(TXT_CALC_SAVED, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    
    savedCalculations = new SavedCalculations();
    
    connect(
	    savedCalculations,
	    SIGNAL( itemClicked(const QString&) ),
	    this,
	    SLOT( calculationHistoryItemClicked(const QString&) ) );
    
    connect(
            savedCalculations,
	    SIGNAL(savedCalculationsCleared()),
	    this,
	    SLOT(resetSavedCalculationNumber()) );

    connect(
            savedCalculations,
	    SIGNAL(itemAlreadyExists()),
	    this,
	    SLOT(handleItemAlreadyExists() ) );

    connect(
            savedCalculations,
	    SIGNAL( itemSaved() ),
	    this,
	    SLOT( handleItemSaved() ) );

    connect(
	    savedCalculations,
	    SIGNAL( reinitializeSave() ),
	    this,
	    SLOT( resetSavedCalculationNumber() ) );

    connect( action, SIGNAL( triggered(bool) ), this, SLOT(showSavedCalculations()) );
    
    MLayout * mainLayout = new MLayout( panel );
    panel->setLayout( mainLayout );

    landscapePolicy = new MLinearLayoutPolicy( mainLayout, Qt::Horizontal );
    portraitPolicy  = new MLinearLayoutPolicy( mainLayout, Qt::Vertical );

    calculationLine = new CalculationLineWidget( this );
    calculationLine->setItemMode( CalculationLineWidget::Single );
    calculationLine->setText( "0" );
    
    MLocale mylocale;

    for( int i = 0; i < 10; i++ )
    {
	QString formattedNumber = mylocale.formatNumber(i);

        digitButtons[i] = createButton(formattedNumber, SLOT(digitClicked()));
        digitButtons[i]->setObjectName( "numberItem" );
    }

    buttonC = new MButton( QString(TXT_CALC_CLEAR)=="!! "? "C" : TXT_CALC_CLEAR );
    buttonBackspace = new MButton();
    buttonBackspace->setIconID( ICO_CALC_BACKSPACE );
    buttonSign = new MButton( QString(TXT_CALC_SIGN)=="!! "? "+/-" : TXT_CALC_SIGN );
    buttonDecimal = new MButton( mylocale.formatNumber(1.2)==""? ".":QString( mylocale.formatNumber(1.2)[1]) );

    buttonDivide = new MButton( QString(TXT_CALC_DIVIDE)=="!! "? "/" : TXT_CALC_DIVIDE );
    //buttonMultiply = new MButton( TXT_CALC_MULTIPLY);
    buttonMultiply = new MButton("*");
    buttonSubtract = new MButton( QString(TXT_CALC_SUBTRACT)=="!! "?"-" : TXT_CALC_SUBTRACT );
    buttonAdd = new MButton( QString(TXT_CALC_ADD)=="!! "?"+" : TXT_CALC_ADD );
    //buttonEquals = new MButton( QString(TXT_CALC_EQUAL)=="!! "?"=" : TXT_CALC_EQUAL );
    buttonEquals = new MButton( "=" );
    buttonAdd->setCheckable( true );
    buttonSubtract->setCheckable( true );
    buttonMultiply->setCheckable( true );
    buttonDivide->setCheckable( true );

    buttonAdd->setObjectName( "operatorItem" );
    buttonSubtract->setObjectName( "operatorItem" );
    buttonMultiply->setObjectName( "operatorItem" );
    buttonDivide->setObjectName( "operatorItem" );
    buttonC->setObjectName( "clearItem" );
    buttonSign->setObjectName( "otherItem" );
    buttonDecimal->setObjectName( "otherItem" );
    buttonEquals->setObjectName( "equalsItem" );
    buttonBackspace->setObjectName( "otherItem" );

    MLayout* operLayout = new MLayout( mainLayout );
    MLayout* numLayout  = new MLayout( mainLayout );

    MGridLayoutPolicy* numLayoutPolicy = new MGridLayoutPolicy( numLayout );
    numLayoutPolicy->addItem( digitButtons[1], 0, 0 );
    numLayoutPolicy->addItem( digitButtons[2], 0, 1 );
    numLayoutPolicy->addItem( digitButtons[3], 0, 2 );
    numLayoutPolicy->addItem( digitButtons[4], 1, 0 );
    numLayoutPolicy->addItem( digitButtons[5], 1, 1 );
    numLayoutPolicy->addItem( digitButtons[6], 1, 2 );
    numLayoutPolicy->addItem( digitButtons[7], 2, 0 );
    numLayoutPolicy->addItem( digitButtons[8], 2, 1 );
    numLayoutPolicy->addItem( digitButtons[9], 2, 2 );
    numLayoutPolicy->addItem( digitButtons[0], 3, 0 );
    numLayoutPolicy->addItem( buttonEquals, 3, 1, 1, 2 );

    numLayout->setLandscapePolicy( numLayoutPolicy );
    numLayout->setPortraitPolicy( numLayoutPolicy );

    MGridLayoutPolicy* operLayoutLandscapePolicy = new MGridLayoutPolicy( operLayout );
    operLayoutLandscapePolicy->addItem( calculationLine, 0, 0, 1, 3 );
    operLayoutLandscapePolicy->addItem( buttonC, 1, 0, 1, 2 );
    operLayoutLandscapePolicy->addItem( buttonBackspace, 1, 2 );
    operLayoutLandscapePolicy->addItem( buttonSubtract, 2, 0 );
    operLayoutLandscapePolicy->addItem( buttonDivide, 2, 1 );
    operLayoutLandscapePolicy->addItem( buttonSign, 2, 2 );
    operLayoutLandscapePolicy->addItem( buttonAdd, 3, 0 );
    operLayoutLandscapePolicy->addItem( buttonMultiply, 3, 1 );
    operLayoutLandscapePolicy->addItem( buttonDecimal, 3, 2 );
    operLayout->setLandscapePolicy( operLayoutLandscapePolicy );

    MGridLayoutPolicy* operLayoutPortraitPolicy = new MGridLayoutPolicy( operLayout );
    operLayoutPortraitPolicy->addItem( calculationLine, 0, 0, 1, 4 );
    operLayoutPortraitPolicy->addItem( buttonSubtract, 1, 0 );
    operLayoutPortraitPolicy->addItem( buttonDivide, 1, 1 );
    operLayoutPortraitPolicy->addItem( buttonC, 1, 2 );
    operLayoutPortraitPolicy->addItem( buttonBackspace, 1, 3 );
    operLayoutPortraitPolicy->addItem( buttonAdd, 2, 0 );
    operLayoutPortraitPolicy->addItem( buttonMultiply, 2, 1 );
    operLayoutPortraitPolicy->addItem( buttonSign, 2, 2 );
    operLayoutPortraitPolicy->addItem( buttonDecimal, 2, 3 );
    operLayout->setPortraitPolicy( operLayoutPortraitPolicy );

    landscapePolicy->addItem( operLayout );
    landscapePolicy->addItem( numLayout );

    portraitPolicy->addItem( operLayout );
    portraitPolicy->addItem( numLayout );

    mainLayout->setLandscapePolicy( landscapePolicy );
    mainLayout->setPortraitPolicy( portraitPolicy );
    makeConnections();
    
    /***********************************************************************************
     * create the save dialog here, this can be reused whenever the user wants to save *
     * calculation                                                                     *
     ***********************************************************************************/
    MWidget* centralWidget = new MWidget;
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical );
//    MLabel* label = new MLabel( TXT_CALC_SAVE_NAME, centralWidget);
    
    saveText = new MTextEdit(
                       MTextEditModel::SingleLine,
		       "",
		       centralWidget);

    saveText->setMaxLength( 30 );
    saveText->setInputMethodCorrectionEnabled( false );

    saveDialog = new MDialog( TXT_CALC_SAVE_TITLE, M::SaveButton | M::CancelButton );
    saveDialog->setCentralWidget( centralWidget );
    saveDialog->setCloseButtonVisible(true);
    centralWidget->setLayout( layout );
//    layout->addItem(label);
    layout->addItem(saveText);

    connect(
            saveDialog,
            SIGNAL(accepted()),
            this,
            SLOT(saveCalcAccepted()));

    connect(
            saveDialog,
            SIGNAL(rejected()),
            this,
            SLOT(saveCalcRejected()));
}

void BasicCalc::makeConnections()
{
    connect(
        buttonC,
        SIGNAL(clicked()),
        this,
        SLOT(clearClicked()) );

    connect(
        buttonBackspace,
        SIGNAL(clicked()),
        this,
        SLOT(backspaceClicked()) );

    connect(
        buttonDecimal,
        SIGNAL(clicked(bool)),
        this,
        SLOT(decimalClicked()) );

    connect(
        buttonDivide,
        SIGNAL(clicked(bool)),
        this,
        SLOT(operatorClicked(bool)));

    connect(
        buttonMultiply,
        SIGNAL(clicked(bool)),
        this,
        SLOT(operatorClicked(bool)));

    connect(
        buttonSubtract,
        SIGNAL(clicked(bool)),
        this,
        SLOT(operatorClicked(bool)));

    connect(
        buttonAdd,
        SIGNAL(clicked(bool)),
        this,
        SLOT(operatorClicked(bool)));

    connect(
        buttonEquals,
        SIGNAL(clicked(bool)),
        this,
        SLOT(equalsClicked()));

    connect(
	buttonSign,
	SIGNAL(clicked(bool)),
	this,
	SLOT(signClicked()));

    connect(
        core,
        SIGNAL(result(QString)),
        this,
        SLOT(showResult(QString)) );
}

MButton* BasicCalc::createButton( const QString text, const char* member )
{
    MButton *button = new MButton(text);
    connect(button, SIGNAL(clicked(bool)), this, member);
    return button;
}

void BasicCalc::keyPressEvent(QKeyEvent *event)
{
    if( event->matches( QKeySequence::Copy ) )
    {
        copy();
	event->accept();
	return;
    }
    
    if( event->matches( QKeySequence::Paste ) )
    {
        paste();
	event->accept();
	return;
    }

    switch( event->key() )
    {
	case Qt::Key_0:
            digitButtons[0]->click();
	    break;
	case Qt::Key_1:
            digitButtons[1]->click();
	    break;
	case Qt::Key_2:
            digitButtons[2]->click();
	    break;
	case Qt::Key_3:
            digitButtons[3]->click();
	    break;
	case Qt::Key_4:
            digitButtons[4]->click();
	    break;
	case Qt::Key_5:
            digitButtons[5]->click();
	    break;
	case Qt::Key_6:
            digitButtons[6]->click();
	    break;
	case Qt::Key_7:
            digitButtons[7]->click();
	    break;
	case Qt::Key_8:
            digitButtons[8]->click();
	    break;
	case Qt::Key_9:
            digitButtons[9]->click();
	    break;
	case Qt::Key_Plus:
	    buttonAdd->click();
	    break;
	case Qt::Key_Minus:
	    buttonSubtract->click();
	    break;
	case Qt::Key_Asterisk:
	    buttonMultiply->click();
	    break;
	case Qt::Key_Slash:
	    buttonDivide->click();
	    break;
	case Qt::Key_Backspace:
	    buttonBackspace->click();
	    break;
	case Qt::Key_Return:
	    buttonEquals->click();
	    break;
	case Qt::Key_Period:
	    buttonDecimal->click();
	    break;
    }
}

void BasicCalc::copy()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( calculationLine->text() );
}

void BasicCalc::paste()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();

    if( text == "" )
	return;

    text.remove( QChar('\n') );
    text.remove( QChar('\t') );
    text.remove( QChar('\r') );
    text.remove( QChar(' ') );
    
    // this is to take care of the scenario where a non-exponential value is being pasted from
    // the clipboard and the value has to be converted to the exponential format
    if( text.count( "." ) > 1 )
    {
	// invalid input need to show a banner and return
	utility->showNotificationBanner( TXT_CALC_NOTE_INVALID, ICO_CALC_CROSS );
	return;
    }

    MLocale locale("en_US");
    text.remove( QChar(locale.formatNumber(1000)[1]) );
    qDebug() << "formatted text after paste = " << text << endl;
    
    QRegExp rx("^\\-?[0-9]+\\.?[0-9]*(e|E|e\\+|E\\+|e\\-|E\\-)?[0-9]{1,2}$");
    int idx=rx.indexIn( text, 0 );
    qDebug() << "idx = " << idx << endl;
    qDebug() << "matched length = " << rx.matchedLength() << endl;
    qDebug() << "text length = " << text.length() << endl;

    if( rx.matchedLength() < text.length() )
    {
	rx.setPattern( "\\-?[0-9]" );
	idx = rx.indexIn( text, 0 );
	if( rx.matchedLength() < text.length() )
	{
	    utility->showNotificationBanner( TXT_CALC_NOTE_INVALID, ICO_CALC_CROSS );
	    return;
	}
    }
    
    utility->convertToExponential( text );
    if( text != "" )
        calculationHistoryItemClicked( text );
}

void BasicCalc::digitClicked()
{
    this->setUnchecked();

    buttonSign->setEnabled( true );
    buttonSign->setObjectName( "otherItem" );

    if( operation != "" )
    {
        QString calculationLineValue = calculationLine->text();
	core->calculationStack()->push( calculationLine->text(), operation );
	operation = "";
    }

    MButton *clickedButton = qobject_cast<MButton *>(sender());
    QString digitValue = numeralMap.value( clickedButton->text() );
    qDebug() << "digit = " << digitValue << endl;

    QString calculationLineValue = calculationLine->text();
    qDebug() << "calculation line value = " << calculationLineValue << endl;

    if( calculationStarted )
    {
        calculationLineValue = "";
        calculationStarted = false;
    }
    
    if( calculationLineValue == "0" )
        calculationLineValue = "";

    if( calculationLineValue == "-0" && digitValue == "0" )
        return;

    if( calcState == RESULT )
    {
        calculationLineValue = "";
        calculationDone = false;
	calcState = DIGIT;
        core->calculationStack()->clear();
    }
    else if( calcState != SAVE )
	calcState = DIGIT;

    if( !calculationLineValue.contains( "." ) )
    {
        if( (!calculationLineValue.startsWith("-") && calculationLineValue.length() >= MAX_VALUE_LENGTH) ||
            (calculationLineValue.startsWith("-") && calculationLineValue.length() > MAX_VALUE_LENGTH ) )
        {
            if( !calculationLineValue.contains( "e" ) )
                showTypingLimitBanner();
            return;
        }
    }
    else
    {
        if( (!calculationLineValue.startsWith("-") && calculationLineValue.length() >= MAX_VALUE_LENGTH + 1) ||
            (calculationLineValue.startsWith("-") && calculationLineValue.length() > MAX_VALUE_LENGTH + 1) )
	{
            if( !calculationLineValue.contains( "e" ) )
                showTypingLimitBanner();
            return;
	}
    }

    if( calculationLineValue.contains( "e" ) )
    {
	if( calcState == RESULT )
	    calculationLineValue = digitValue;
    }
    else if( calcState == SAVE &&
              !(buttonAdd->isChecked() ||
	        buttonSubtract->isChecked() ||
	        buttonMultiply->isChecked() ||
	        buttonDivide->isChecked()) )
    {
        core->calculationStack()->clear();
        storedReference.clear();
        lastResult = "";
	lastSavedResult = "";
	calculationLineValue = digitValue;
	calcState = DIGIT;
        calculationDone = false;
    }
    else
	calculationLineValue += digitValue;
    qDebug() << "calculation line value after modification = " << calculationLineValue << endl;

    calculationLine->setText( calculationLineValue );
}

void BasicCalc::showTypingLimitBanner()
{
    if( exceedingLimitBannerDisplayed )
        return;

    utility->showNotificationBanner( TXT_CALC_NOTE_TYPING, ICO_CALC_CROSS );

    QTimer* avoidNotificationTimer = new QTimer();
    connect(
            avoidNotificationTimer,
            SIGNAL(timeout()),
            this,
            SLOT(avoidNotificationTimeout()) );
   avoidNotificationTimer->setSingleShot( true );
   avoidNotificationTimer->start( CALC_BANNER_LONG_TIMER );

    exceedingLimitBannerDisplayed = true;
}

void BasicCalc::avoidNotificationTimeout()
{
    QTimer* timer = qobject_cast<QTimer*>(sender());
    if( timer )
        exceedingLimitBannerDisplayed = false;
}


void BasicCalc::backspaceClicked()
{
    qDebug() << Q_FUNC_INFO << endl;
    buttonSign->setEnabled( true );
    buttonSign->setObjectName( "otherItem" );

    if(  buttonAdd->isChecked() ||
         buttonSubtract->isChecked() ||
         buttonMultiply->isChecked() ||
         buttonDivide->isChecked() )
    {
        // set all the operator buttons to unchecked
        this->setUnchecked();
	// pop operator first
        if( !core->calculationStack()->isEmpty() )
            core->calculationStack()->pop_back();
	// pop operand next
        if( !core->calculationStack()->isEmpty() )
	{
	    // need to remove the embedded brackets "(" and ")" if they are present
	    if( core->calculationStack()->last() == ")" )
		core->calculationStack()->pop_back();
            if( !core->calculationStack()->isEmpty() )
		core->calculationStack()->pop_back();
	    if( !core->calculationStack()->isEmpty() && 
		core->calculationStack()->last() == "(" )
		core->calculationStack()->pop_back();
	}
        // set the operation to NULL
        operation = "";
	calcState = SAVE; // Although this is not intended to take the
	                  // calculator to state of save, the state can be
			  // reused for bypassing the default appending after digit click
	calculationStarted = false;
        return;
    }
    
    QString calculationLineValue = calculationLine->text();
    qDebug() << "calculationLineValue (before):" << calculationLineValue << endl;

    // If the calculation line is an exponential value, then the 
    // value shouldn't be editable.
    QString temp = calculationLine->text();
    utility->convertToExponential( temp );
    if( temp.contains( "e" ) )
	return;

    if( !core->calculationStack()->isEmpty() )
    {
	if( (calculationLineValue.length() == 1) || 
	    (calculationLineValue.length() == 2 && calculationLineValue.startsWith( "-" )) )
	{
            calculationLineValue = core->calculationStack()->at( core->calculationStack()->size() - 2 );
	    if( calculationLineValue == ")" )
		calculationLineValue = core->calculationStack()->at( 
                                                  core->calculationStack()->size() - 3 );
            QString op = core->calculationStack()->last();
            if( buttonAdd->text() == operationMap.key(op) )
                buttonAdd->setChecked( true );                                        
            if( buttonSubtract->text() == operationMap.key(op) )
                buttonSubtract->setChecked( true );
            if( buttonMultiply->text() == operationMap.key(op) )
                buttonMultiply->setChecked( true );
            if( buttonDivide->text() == operationMap.key(op) )
                buttonDivide->setChecked( true );
            calcState = OPERATOR;
            calculationStarted = true;
            buttonSign->setEnabled( false );
            //buttonSign->setObjectName( "disabledItem" );
    	}
    	else
	{
            // TERMINATION CONDITION FOR HISTORY
	    qDebug() << "Reached backspace termination condition for history 1" << endl;
	    if( !storedReference.isEmpty() )
	        saveCalculationTo( HISTORY );
            storedReference.clear();
            lastResult = "";
	    
	    temp.truncate( temp.length() - 1 );
	    calculationLineValue = temp;
	    calcState = DIGIT;
	    calculationStarted = false;
	}
    }
    else if( calculationLineValue != "0" )
    {
	calcState = DIGIT;
	
	if( (calculationLineValue.length() == 1) || 
	    (calculationLineValue.length() == 2 && calculationLineValue.startsWith( "-" ) ) ||
	    (calculationLineValue == "-0.") )
	{
	    calculationLineValue = "0";
	}
	else
	{
	    temp.truncate( temp.length() - 1 );
	    calculationLineValue = temp;
	}
        
	if( !storedReference.isEmpty() )
	{
	    // TERMINATION CONDITION FOR HISTORY
	    qDebug() << "Reached backspace termination condition for history 2" << endl;
	    saveCalculationTo( HISTORY );
            storedReference.clear();
	    lastSavedResult = "";
            lastResult = "";
	}

	calculationStarted = false;
    }

    qDebug() << "calculationLineValue (after):" << calculationLineValue << endl;
    calculationLine->setText( calculationLineValue );
}

void BasicCalc::clearClicked()
{
    qDebug() << Q_FUNC_INFO << endl;
    this->setUnchecked();
    buttonSign->setEnabled( true );
    buttonSign->setObjectName( "otherItem" );
    core->calculationStack()->clear();
    QString calculationLineValue("0");
    calculationLine->setText( calculationLineValue );
    // TERMINATION CONDITION FOR HISTORY
    if( !storedReference.isEmpty() && storedReference.count() > 1 )
        saveCalculationTo( HISTORY );
    storedReference.clear();
    lastResult = "";
    lastSavedResult = "";
    operation = ""; // clear any pending operations to be done.

    // Also need to clear the history and save candidate items
    saveCandidate["result"] = "";
}

void BasicCalc::setUnchecked()
{
    buttonAdd->setChecked( false );
    buttonSubtract->setChecked( false );
    buttonMultiply->setChecked( false );
    buttonDivide->setChecked( false );
}

void BasicCalc::operatorClicked(bool checked)
{
    qDebug() << Q_FUNC_INFO << endl; 

    MButton *clickedButton = qobject_cast<MButton *>(sender());
    this->setUnchecked();
    clickedButton->setChecked( checked );
    buttonSign->setEnabled( !checked );
//    buttonSign->setObjectName( checked? "disabledItem" : "otherItem" );
    
    if( !checked )
    {
	// pop operator first
        if( !core->calculationStack()->isEmpty() )
            core->calculationStack()->pop_back();
	// pop operand next
        if( !core->calculationStack()->isEmpty() )
	{
	    // need to remove the embedded brackets "(" and ")" if they are present
	    if( core->calculationStack()->last() == ")" )
		core->calculationStack()->pop_back();
            if( !core->calculationStack()->isEmpty() )
		core->calculationStack()->pop_back();
	    if( !core->calculationStack()->isEmpty() && 
		core->calculationStack()->last() == "(" )
		core->calculationStack()->pop_back();
	}
        calculationStarted = false;
	operation = "";
	return;
    }
    
    QString op = operationMap.value(clickedButton->text());
    qDebug() << "op = " << op << endl;

    if( calcState == SAVE )
    {
	core->calculationStack()->push( calculationLine->text(), op );
	if( lastSavedResult != "" )
            calculationLine->setText( lastSavedResult );
	else
	{
	    QString temp = calculationLine->text();
            calculationLine->setText( temp );
	}
	operation = "";
        calcState = OPERATOR;
        calculationStarted = true;
	return;
    }
    
    QString calculationLineValue = calculationLine->text();
    qDebug() << "calculationLineValue = " << calculationLineValue << endl;

    chopOffCalculationLine();


    if( core->calculationStack()->isEmpty() == false )
    {
        qDebug() << "Last item in the calculation stack is " << core->calculationStack()->last() << endl;
        if( op != core->calculationStack()->last() )
        {
            if( this->isBasicOperator( op ) &&
                this->isBasicOperator( core->calculationStack()->last() ) && 
		calcState == OPERATOR )
            {
                core->calculationStack()->pop_back();
                core->calculationStack()->push_back( op );
            }
            else
	    {
		operation = op;
		core->calculationStack()->push( calculationLine->text() );
		core->calculate();
	    }
        }
        else
	{
	    operation = op;
	    core->calculationStack()->push( calculationLine->text() );
	    core->calculate();
	}
    }
    else
    {
	qDebug() << "push both calculation line and op" << endl;
        // push both calculationLineValue and operator
	core->calculationStack()->push( calculationLine->text(), op );
	operation = "";
    }
    calcState = OPERATOR;
    calculationStarted = true;
}

void BasicCalc::equalsClicked()
{
    buttonSign->setEnabled( true );
    buttonSign->setObjectName( "otherItem" );

    if( calcState != RESULT )
    {
	this->setUnchecked();
	chopOffCalculationLine();
        QString calculationLineValue = calculationLine->text();
        if(  operation != "" )
        {
	    core->calculationStack()->push( calculationLine->text(), operation );
        }
	core->calculationStack()->push( calculationLine->text() );
        calcState = RESULT;
	operation = "";
	calculationStarted = false;
	core->calculate();
    }
}

void BasicCalc::signClicked()
{
    if(  buttonAdd->isChecked() ||
         buttonSubtract->isChecked() ||
         buttonMultiply->isChecked() ||
         buttonDivide->isChecked() )
    {
        // set all the operator buttons to unchecked
        this->setUnchecked();
	// pop operator first
        if( !core->calculationStack()->isEmpty() )
            core->calculationStack()->pop_back();
	// pop operand next
        if( !core->calculationStack()->isEmpty() )
	{
	    // need to remove the embedded brackets "(" and ")" if they are present
	    if( core->calculationStack()->last() == ")" )
		core->calculationStack()->pop_back();
            if( !core->calculationStack()->isEmpty() )
		core->calculationStack()->pop_back();
	    if( !core->calculationStack()->isEmpty() && 
		core->calculationStack()->last() == "(" )
		core->calculationStack()->pop_back();
	}
        // set the operation to NULL
        operation = "";
        calcState = DIGIT;
        calculationStarted = false;
    }

    QString calculationLineValue = calculationLine->text();

    if( calculationLineValue == "0" )
	return; // nothing to do :)
    
    if( calculationLineValue.at(0) == QChar('-') )
    {
	calculationLineValue.remove(0,1);
    }
    else
    {
	calculationLineValue.insert( 0, QString("-" ) );
    }
    calcState = DIGIT;
    
    calculationLine->setText( calculationLineValue );
    //calculationLineText = calculationLineValue;
}

void BasicCalc::decimalClicked()
{
    qDebug() << Q_FUNC_INFO << endl;
    buttonSign->setEnabled( true );
    buttonSign->setObjectName( "otherItem" );
    QString calculationLineValue = this->calculationLine->text();
    if( calcState == RESULT )
    {
	calculationLineValue = "0.";
        this->calculationLine->setText( calculationLineValue);
	calcState = DIGIT;
	calculationStarted = false;
	return;
    }
    if(  buttonAdd->isChecked() ||
         buttonSubtract->isChecked() ||
         buttonMultiply->isChecked() ||
         buttonDivide->isChecked() )
    {
	this->setUnchecked();
        if( operation != "" )
        {
    	    core->calculationStack()->push( calculationLine->text(), operation );
    	    operation = "";
        }
	calculationLineValue = "0.";
        this->calculationLine->setText( calculationLineValue);
	calcState = DIGIT;
	calculationStarted = false;
    }
    else if( calculationLineValue.contains( "e" ) ) // Ignore decimal point if the value is exponential
    {
	return;
    }
    else if( !calculationLineValue.contains( "." ) )
    {
        if( (!calculationLineValue.startsWith("-") && calculationLineValue.length() >= MAX_VALUE_LENGTH) ||
            (calculationLineValue.startsWith("-") && calculationLineValue.length() > MAX_VALUE_LENGTH ) )
	    return; // no need to do anything in this case, just return 
	
        calculationLineValue += ".";
        this->calculationLine->setText( calculationLineValue);
	calcState = DIGIT;
	calculationStarted = false;
        buttonSign->setEnabled( true );
	buttonSign->setObjectName( "otherItem" );
    }
}

void BasicCalc::showResult( const QString& res )
{
    QString temp = res;
    qDebug() << Q_FUNC_INFO << "result = " << res << endl;

    if( res.length() <= 0 )
	return;
    
    QRegExp rx("[1-9]+");
    int idx=rx.indexIn( temp, 0 );
    qDebug() << "idx = " << idx << endl;
    if( rx.matchedLength() <= 0 )
    {
	temp = "0";
    }


    qDebug() << "**** Add to stored reference ***" << endl;
    for( QStringList::iterator i = core->calculationStack()->begin(); i != core->calculationStack()->end(); i++ )
    {
	if( !storedReference.isEmpty() && lastResult != "" )
	{
	    lastResult = "";
	    // if the result has already been calculated, we need to head past the first operand
	    if( *i == "(" )
	    {
		// this is the case for negative number
		// go beyond '('
		i++;
		// go beyond '-xxxxx'
		if( i != core->calculationStack()->end() )
		    i++;
		// go beyond ')'
		if( i != core->calculationStack()->end() )
		    i++;

                if( i == core->calculationStack()->end() )
                {
		    break;
		}
	    }
	    else if( !this->isBasicOperator( *i ) )
	    {
		// this means that iterator is currently referencing to a positive number
		continue;
	    }
	}
	
	if( !storedReference.isEmpty() && 
	    !this->isBasicOperator( storedReference.at( storedReference.count() - 2 ) ) &&
	    *i == "0" )
	    continue;

	qDebug() << *i << endl;
	if( !this->isBasicOperator(*i) && 
	    *i != "(" && *i != ")"  )
	{
	    QString expFormat = *i;
            utility->convertToExponential( expFormat );
	    storedReference << expFormat << " ";
	}
	else
	{
	    // LOGIC FOR DISPLAYING CORRECT MATHEMATICAL EXPRESSION (BODMAS rule)
	    // THIS APPLIES TO CASES LIKE OR CHAINED OPERATIONS WITH COMBINATION OF FOLLOWING
	    // 1. A+B*C
	    // 2. A+(-B)*C
	    // 3. (-A)+B*C
	    // 4. (-A)+(-B)*C
	    // 5. A-B/C
	    // etc. where the following conditions are met. The brackets should be added to the beginning and end
	    //      i.  the previous operator is either of "+" or "-"
	    //      ii. the current operator is either of "*" or "/"
	    //
		
	    QString prevOperator = "";
            
	    qDebug() << "current stored reference: ";
	    for( QStringList::iterator myit = storedReference.begin(); myit!= storedReference.end(); myit++ )
		qDebug() << *myit << " | ";

	    qDebug() << endl;
	    
	    if( storedReference.count() > 3*2 )
	    {
		if( storedReference.at( storedReference.count() - 2 ) == ")" )
		    prevOperator = storedReference.at( storedReference.count() - 4*2 );
		    //    cases handled by this assignment: the operator extracted might be anything "+", "-", "*" or "/"
		    //    "+" is taken as an example below
		    //    
		    //    ---------------------------------------
		    //    | A |   |  + |   | ( |   | -B |   | ) |
		    //    ---------------------------------------
		    //    
		    //    ---------------------------------------------------
		    //    | ( |   | -A |   | ) | + |   | ( | -B |   | ) |   |
		    //    ---------------------------------------------------
		    //    
		else
		    prevOperator = storedReference.at( storedReference.count() - 2*2 );
		    //    cases handled by this assignment: the operator extracted might be anything "+", "-", "*" or "/"
		    //    "+" is taken as an example below
		    //    
		    //    ------------------------------------------
		    //    | ( |   | -A |   | ) |   | + |   | B |   |
		    //    ------------------------------------------
		    //    
	    }
	    else if( storedReference.count() == 3*2 && isBasicOperator( storedReference.at( storedReference.count() - 2*2 ) ) )
	    {
		// case handled by this assignment: the operator extracted might be anything "+", "-", "*" or "/"
		// "+" is taken as an example below
		// 
		// -------------------------
		// | A |   | + |   | B |   |
		// -------------------------
		//
		// NOTE: the following expression will be eliminated because of the check above
		//
		// --------------
		// | ( | -A | ) |
		// --------------
		//
		prevOperator = storedReference.at( storedReference.count() - 2*2 );
	    }
	    
	    if( (prevOperator == "+" || prevOperator == "-") &&
		(*i == "*" || *i == "/" ) )
            {
		storedReference.prepend( " " );
		storedReference.prepend( "(" );
		storedReference.append( ")" );
		storedReference.append( " " );
	    }
	    
	    // else NO NEED TO ADD ANY BRACKETS
	    
	    storedReference << *i
		            << " ";
	}
    }

    int i = res.indexOf( "." );
    
    QString banner;
    QString formattedBanner;

    if( (i < 0) &&
	((!res.startsWith("-") && res.length() > MAX_DISPLAY_VALUE) ||
         (res.startsWith("-") && res.length() > MAX_DISPLAY_VALUE+1)) )
    {
	banner = TXT_CALC_NOTE_CALC_LIMIT;
	formattedBanner = QString("<style>red{color:#FF0000;}</style>") + 
		 QString("<red>") +
		 banner +
		 QString("</red> ");
	storedReference.push_front( formattedBanner );
	calcState = RESULT;
        calculationDone = true;
    }
    else if( (i > 0) &&
	     ((!res.startsWith("-") && i > MAX_DISPLAY_VALUE) ||
              (res.startsWith("-") && i > MAX_DISPLAY_VALUE+1)) )
    {
	banner = TXT_CALC_NOTE_CALC_LIMIT;
	formattedBanner = QString("<style>red{color:#FF0000;}</style>") + 
		 QString("<red>") +
		 banner +
		 QString("</red> ");
	storedReference.push_front( formattedBanner );
	calcState = RESULT;
        calculationDone = true;
    }
    else if( res.contains("Error") )
    {
	banner = TXT_CALC_NOTE_INDICATION;
	formattedBanner = QString("<style>red{color:#FF0000;}</style>") + 
		 QString("<red>") + 
		 banner +
		 QString("</red> ");
        calculationDone = true;
	if( calcState != SAVE )
	    storedReference.push_front( formattedBanner );
    }
    else
    {
	banner = "";
	this->calculationLine->setText( res );
	lastResult = temp;
	lastSavedResult = temp;
    }
    
    if( banner != "" )
        utility->showNotificationBanner( banner, ICO_CALC_CROSS );

    if( calcState == RESULT || banner != "" )
    {
	qDebug() << "save calculation to history" << endl;
	// save only if the stored reference is non-empty
	if( !storedReference.isEmpty() )
	    saveCalculationTo( HISTORY );
	storedReference.clear();
	lastResult = "";
	lastSavedResult = "";
    }

    core->calculationStack()->clear();
    if( this->operation == "" )
        calculationDone = true;
}

void BasicCalc::chopOffCalculationLine()
{
    QString calculationLineValue = calculationLine->text();
    QRegExp rx("[1-9]+");
    int idx=rx.indexIn( calculationLineValue, 0 );
    qDebug() << "idx = " << idx << endl;
    if( rx.matchedLength() <= 0 )
    {
//	calculationLineText = 
		calculationLineValue = "0";
        calculationLine->setText( calculationLineValue );
    }

    QStringList list = calculationLineValue.split( "." );
    if( list.count() > 1 )
    {
	idx=rx.indexIn( list.last(), 0 );
        qDebug() << "idx = " << idx << endl;
	if( rx.matchedLength() <= 0 )
	{
	    //calculationLineText = 
		    calculationLineValue = list.at(0);
            calculationLine->setText( calculationLineValue );
	}
    }
}

bool BasicCalc::isBasicOperator( const QString& op )
{
    return ( op == "+" || op == "-" || op == "*" || op == "/" )? true:false;
}

void BasicCalc::saveFromHistory( CalculationItem& calcItem )
{
    saveCandidateFromHistory = calcItem;
    saveCalculationFromHistory = true;
    QSettings settings( "Nokia", "Calculator" );
    qDebug() << "save calculation from history? " << saveCalculationFromHistory << endl;
    showSaveCalcDialog();
}

void BasicCalc::showSaveCalcDialog()
{
    saveInProgress = false;
    saveText->setPrompt(TXT_CALC_SAVE_DEF);
    saveDialog->appear();
}

void BasicCalc::saveCalcAccepted()
{
    qDebug() << "BasicCalc::saveCalcAccepted()" << endl;
    
    if( savedCalculations->count() == MAX_SAVED_CALCULATION_ITEMS )
    {
	utility->showNotificationBanner( TXT_CALC_SAVE_EXCEED, ICO_CALC_CROSS );
	return;
    }
    
    if( saveInProgress )
	return;
    
    saveInProgress = true;
    if( !saveCalculationFromHistory )
    {
        if( calcState != RESULT )
        {
            calcState = SAVE;
            if( !(buttonAdd->isChecked() ||
                 buttonSubtract->isChecked() ||
                 buttonMultiply->isChecked() ||
                 buttonDivide->isChecked()) )
            {
	        core->calculationStack()->push( calculationLine->text() );
                core->calculate();
	    }
	}
    }
    else
    {
	if( saveText->text() == "" )
	    saveCandidateFromHistory["name"] = saveText->prompt();
	else
	    saveCandidateFromHistory["name"] = saveText->text();
    }
    if( !saveCalculationTo( SAVED ) )
    {
    }
    savedCalculations->updateContent();
    saveCalculationFromHistory = false;
    saveInProgress = false;
	saveText->setText("");
}

void BasicCalc::saveCalcRejected()
{
    qDebug() << "BasicCalc::saveCalcRejected()" << endl;
    saveCalculationFromHistory = false;
	saveText->setText("");
}

void BasicCalc::calculationHistoryItemClicked( const QString& result )
{
    if( operation != "" )
    {
        QString calculationLineValue = calculationLine->text();
	core->calculationStack()->push( calculationLine->text(), operation );
	operation = "";
    }

    this->calculationLine->setText( result );
    calcState = DIGIT;
    this->setUnchecked();
    buttonSign->setEnabled( true );
    buttonSign->setObjectName( "otherItem" );
    MApplicationPage::appear();
}

bool BasicCalc::saveCalculationTo( SaveTarget saveInto )
{
    if( saveCalculationFromHistory )
    {
	savedCalculations->addItem( saveCandidateFromHistory );
	return true;
    }
    // Prepare ittems to be saved to the file 
    CalculationItem calcItem;
    
    prepareCalculationToSave( calcItem );

    // add this item to calculation history list
    // Proceed only if there's a saved result
    if( saveInto == HISTORY )
    {
        if( lastSavedResult != "" )
	    calculationHistory->addItem( calcItem );

	saveCandidate = calcItem;

        calculationHistory->updateContent();
    }
    else if( saveInto == SAVED )
    {
	if( lastSavedResult != "" )
	{
	    if( saveText->text() == "" )
		calcItem["name"] = saveText->prompt();
	    else
                calcItem["name"] = saveText->text();
	    qDebug() << "use calcitem for saving" << endl;
	    qDebug() << "input = " << calcItem["input"] << ", result = " << calcItem["result"] << endl;
	    if( calcItem["result"] != "" && 
		calcItem["input"] != "0" && 
		calcItem["input"] != "0 " )
	        savedCalculations->addItem( calcItem );
	    else
		return false;
	}
	else
	{
	    if( saveText->text() == "" )
		saveCandidate["name"] = saveText->prompt();
	    else
	        saveCandidate["name"] = saveText->text();
	    qDebug() << "use save candidate for saving" << endl;
	    qDebug() << "input = " << saveCandidate["input"] << ", result = " << saveCandidate["result"] << endl;
	    if( saveCandidate["result"] != "" && 
		saveCandidate["input"] != "0" && 
		saveCandidate["input"] != "0 " )
	        savedCalculations->addItem( saveCandidate );
	    else
		return false;
	}
    }
    return true;
}

void BasicCalc::prepareCalculationToSave( CalculationItem& calcItem )
{
    // 1) No name to this calculation item yet as it is going to "History"
    // "Nomenclature of this child will be done if it is saved" :)
    calcItem["name"] = "";
    
    // 2) Date & Time
    MCalendar calendar;
    MLocale locale;
    QDateTime qdt = calendar.qDateTime(Qt::UTC);
    QString dateTimeString = qdt.toString( "hh:mm ap MMM d, yyyy" );
    qDebug() << "datetimestring=" << dateTimeString << endl;
    calcItem["datetime"] = dateTimeString;
    
    // 3) last stored reference
    QString reference;

    if( storedReference.count() > 0 )
    {
        for( QStringList::iterator i = storedReference.begin(); i!= storedReference.end(); i++ )
            reference += *i;
    }
    else
    {
	// 
	// The storedReference doesn't seem to have got constructed.
	// Construct the new storedReference using the tempStore and the calculation stack
	// 
	QStringList tempStore;
        for( QStringList::iterator i = core->calculationStack()->begin(); i != core->calculationStack()->end(); i++ )
	{
	    if( !this->isBasicOperator(*i) && 
	        *i != "(" && *i != ")"  )
	    {
	        QString expFormat = *i;
                utility->convertToExponential( expFormat );
	        tempStore += expFormat + QString(" ");
	    }
	    else
	    {
		QString prevOperator = "";
		// NOTE: In the logic below, the comparisons require 2*n iterations because of the space introduced
		// for every numeral/bracket/operator
	        if( tempStore.count() > 3*2 )
	        {
		    if( tempStore.at( tempStore.count() - 2 ) == ")" )
		        prevOperator = tempStore.at( tempStore.count() - 4*2 );
        		    //    cases handled by this assignment: the operator extracted might be anything "+", "-", "*" or "/"
        		    //    "+" is taken as an example below
        		    //    
        		    //    ---------------------------------------
        		    //    | A |   |  + |   | ( |   | -B |   | ) |
        		    //    ---------------------------------------
        		    //    
        		    //    ---------------------------------------------------
        		    //    | ( |   | -A |   | ) | + |   | ( | -B |   | ) |   |
        		    //    ---------------------------------------------------
        		    //    
        	    else
        		prevOperator = tempStore.at( tempStore.count() - 2*2 );
        		    //    cases handled by this assignment: the operator extracted might be anything "+", "-", "*" or "/"
        		    //    "+" is taken as an example below
        		    //    
        		    //    ------------------------------------------
        		    //    | ( |   | -A |   | ) |   | + |   | B |   |
        		    //    ------------------------------------------
        		    //    
        	}
        	else if( tempStore.count() == 3*2 && isBasicOperator( tempStore.at( tempStore.count() - 2*2 ) ) )
        	{
        	    // case handled by this assignment: the operator extracted might be anything "+", "-", "*" or "/"
        	    // "+" is taken as an example below
        	    // 
        	    // -------------------------
        	    // | A |   | + |   | B |   |
        	    // -------------------------
        	    //
        	    // NOTE: the following expression will be eliminated because of the check above
        	    //
        	    // --------------
        	    // | ( | -A | ) |
        	    // --------------
        	    //
        	    prevOperator = tempStore.at( tempStore.count() - 2*2 );
        	}
        	    
        	if( (prevOperator == "+" || prevOperator == "-") &&
        	(*i == "*" || *i == "/" ) )
                {
        	    tempStore.prepend( " " );
        	    tempStore.prepend( "(" );
        	    tempStore.append( ")" );
        	    tempStore.append( " " );
        	}
        	    
        	// else NO NEED TO ADD ANY BRACKETS
        	    
        	tempStore << *i
        		   << " ";
	    }
	}

        for( QStringList::iterator i = tempStore.begin(); i!= tempStore.end(); i++ )
            reference += *i;
    }
    calcItem["input"] = reference;
    
    // 4) last result
    calcItem["result"] = lastSavedResult;

    // 5) internal index used for deletion
    QString index = locale.formatDateTime( calendar, "%I:%M:%S %p %b %e, %Y" );
    calcItem["index"] = index;
}

void BasicCalc::retranslateUi()
{
    setTitle(TXT_CALC_TITLE );
    if (!isContentCreated())
	return;

    this->clearActions();
    
    MAction* action;
    
    action = new MAction( TXT_CALC_COPY, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), this, SLOT( copy() ) );
    
    action = new MAction( TXT_CALC_PASTE, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered() ), this, SLOT( paste() ) );
    
    action = new MAction(TXT_CALC_SAVE_ME, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect(
            action,
            SIGNAL(triggered(bool)),
            this,
            SLOT(showSaveCalcDialog()));
    
    action = new MAction(TXT_CALC_HISTORY, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), this, SLOT(showCalculationHistory()) );

    action = new MAction(TXT_CALC_SAVED, this );
    action->setLocation( MAction::ApplicationMenuLocation );
    this->addAction( action );
    connect( action, SIGNAL( triggered(bool) ), this, SLOT(showSavedCalculations()) );

    MLocale mylocale;

    for( int i=0; i < 10; i++ )
    {
	QString formattedNumber = mylocale.formatNumber(i);
        digitButtons[i]->setText( formattedNumber );
    }
    buttonC->setText( QString(TXT_CALC_CLEAR)=="!! "? "C" : TXT_CALC_CLEAR );
    buttonSign->setText( QString(TXT_CALC_SIGN)=="!! "? "+/-" : TXT_CALC_SIGN );
    buttonDivide->setText( QString(TXT_CALC_DIVIDE)=="!! "? "/" : TXT_CALC_DIVIDE );
    buttonMultiply->setText( QString(TXT_CALC_MULTIPLY)=="!! "? "*" : TXT_CALC_MULTIPLY );
    buttonSubtract->setText( QString(TXT_CALC_SUBTRACT)=="!! "?"-" : TXT_CALC_SUBTRACT );
    buttonAdd->setText( QString(TXT_CALC_ADD)=="!! "?"+" : TXT_CALC_ADD );
    buttonDecimal->setText( mylocale.formatNumber(1.2)==""? ".":QString( mylocale.formatNumber(1.2)[1]) );
    buttonEquals->setText( QString(TXT_CALC_EQUAL)=="!! "?"=" : TXT_CALC_EQUAL );
    saveDialog->setTitle( TXT_CALC_SAVE_TITLE );
    saveText->setPrompt(TXT_CALC_SAVE_DEF);
    QString value = calculationLine->text();
    calculationLine->setText( "0");
    calculationLine->setText( value );
}
                                                
void BasicCalc::resetSavedCalculationNumber()
{
}

void BasicCalc::handleItemAlreadyExists()
{
    qDebug() << Q_FUNC_INFO << endl;
    utility->showNotificationBanner( TXT_CALC_SAVE_NOT, ICO_CALC_CROSS );
}

void BasicCalc::handleItemSaved()
{
}

void BasicCalc::showCalculationHistory()
{
    calculationHistory->appear( KeepWhenDone );
}

void BasicCalc::showSavedCalculations()
{
    savedCalculations->appear( KeepWhenDone );
}

