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


#ifndef BASICCALC_H
#define BASICCALC_H

#include <MApplicationPage>
#include <MLabel>
#include <MButton>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MSceneManager>
#include <MCalendar>

#include <MAction>

#include <MButtonGroup>

#include "calcconstants.h"
#include "calcutilities.h"
#include "coreinterface.h"
#include "savedcalculations.h"
#include "calculationhistory.h"

#include "calculationlinewidget.h"

#include <MDialog>
#include <MTextEdit>
#include <MInfoBanner>
#include <MLocale>
#include <QTimer>
#include <QList>
#include <QMap>
#include <QGraphicsLinearLayout>
#include <QSettings>
#include <QClipboard>

#include <iostream>
#include <sstream>

typedef QMap<QString, QString> KeyMap;
typedef QMap<QString, QString>::const_iterator KeyMapIterator;

/**
 \class BasicCalc 
 \brief This class creates and handles the UI interactions for the Basic Calculator.
*/
class BasicCalc : public MApplicationPage
{
    Q_OBJECT
    enum SaveTarget { HISTORY, SAVED };
public:
    /** 
      \brief Constructs a basic calculator  
    */
    BasicCalc();
    /**
      \brief Destructor
    */
    ~BasicCalc();
    /** 
      \brief Creates content for basic calculator  
    */
    virtual void createContent();
    /** 
      \brief save calculation to a \a target where target might be HISTORY or SAVED page
    */    
    bool saveCalculationTo(SaveTarget saveInto);
    /** 
      \brief prepares the \a calcItem to be saved
    */    
    void prepareCalculationToSave( CalculationItem& calcItem );

protected:
    virtual void keyPressEvent(QKeyEvent *event);

	 virtual void retranslateUi();

    
signals:
    /** 
      \brief Signal to handle error scenarios with an \a error string
    */
     void handleErrors( const QString& error );
     
public slots:
    /** 
      \brief Slot to handle the click of a digit [0-9] button
    */
    void digitClicked();
    /** 
      \brief Slot to handle the click of backspace button
    */
    void backspaceClicked();
    /** 
      \brief Slot to handle the click of clear button
    */
    void clearClicked();
    /** 
      \brief Slot to handle the click of operator buttons (+, -, *, /)
    */
    void operatorClicked( bool checked = false );
    /** 
      \brief Slot to handle the click of result/equals button
    */
    void equalsClicked();
    /** 
      \brief Slot to handle the click of sign (+/-) button 
    */
    void signClicked();
    /** 
      \brief Slot to handle the click of decimal button 
    */
    void decimalClicked();
    
    /** 
      \brief Slot to show the \a result in the calculation line
    */
    void showResult( const QString& result );
    /** 
      \brief Slot to show the save calculation dialog when the toolbar save item is clicked
    */
    void showSaveCalcDialog();
    /** 
      \brief Slot to handle the click of Ok button in the save calculation dialog
    */
    void saveCalcAccepted();
    /** 
      \brief Slot to handle the click of Cancel/Close button in the save calculation dialog
    */
    void saveCalcRejected();
    /** 
      \brief Slot to enable re-display of the banner 
    */
    void avoidNotificationTimeout();
    /** 
      \brief Slot to handle click of a history item from the history page
    */    
    void calculationHistoryItemClicked( const QString& );

    void handleItemSaved();

    void handleItemAlreadyExists();
    
    /**
      \brief initiates a save on a \a calcItem from calculation history
    */
    void saveFromHistory( CalculationItem& calcItem );

    void showSavedCalculations();
    void showCalculationHistory();

    void resetSavedCalculationNumber();
    void copy();
    void paste();

    


private:
    /** 
      \brief Common function to create digit button with \a text and register a \a handler function for the \a button
    */
    MButton* createButton( const QString text, const char* handler );
    /** 
      \brief Create connections for the signals and the slots
    */
    void makeConnections();
    /** 
      \brief Check if the \a oper operator is a basic operator or not
    */
    bool isBasicOperator( const QString& oper );
    /** 
      \brief Disable all the buttons
    */
    void disableAll();
    /** 
      \brief Enable all the buttons
    */
    void enableAll();
    /**
      \brief Unchecks all the +,-,*,/ operator buttons if they were selected
     */
    void setUnchecked();

    /**
      \brief Shows the typing limit notification banner
     */
    void showTypingLimitBanner();

    /**
      \brief Do the chopping off 
    */
    void chopOffCalculationLine();

	 /*
	  *\brief does the operator and operand mapping
	  */
	void init();

    /** 
      \brief The calculation bar which displays the ongoing calculation in the calculator in both portrait and landscape
    */
    CalculationLineWidget* calculationLine;
    /** 
      \brief Clear key to erase the calculation line
    */
    MButton* buttonC;
    /** 
      \brief Backspace key to erase last digit one at a time
    */
    MButton* buttonBackspace;
    /** 
      \brief The +/- key to toggle between negative and positive input
    */
    MButton* buttonSign;
    /** 
      \brief Decimal number input key
    */
    MButton* buttonDecimal;
    /** 
      \brief Calculates the reciprocal of the value in calculation line
    */
    MButton* buttonReciprocal;
    /** 
      \brief perform x^y power operation
    */
    MButton* buttonPower;
    /** 
      \brief Calculates the square root of the value in calculation line
    */
    MButton* buttonSqrt;
    /** 
      \brief Calculates the percentage
    */
    MButton* buttonPercentage;
    /** 
      \brief Division operation
    */
    MButton* buttonDivide;
    /** 
      \brief Multiplication operation
    */
    MButton* buttonMultiply;
    /** 
      \brief Subtraction operation
    */
    MButton* buttonSubtract;
    /** 
      \brief Addition operation
    */
    MButton* buttonAdd;
    /** 
      \brief Display result key
    */
    MButton* buttonEquals;
    /** 
      \brief The digits 0-9
    */
    MButton* digitButtons[10];

    /** 
      \brief Panel which contains all the widgets needed for the calculator in landscape and portrait
    */
    QGraphicsWidget* panel;
    
    /** 
      \brief Landscape and portrait layout policies for rendering calculator UI in both the orientations
    */
    MLinearLayoutPolicy* landscapePolicy;
    MLinearLayoutPolicy* portraitPolicy;

    /** 
      \brief Status of the current calculation
    */
    bool calculationDone;
    /** 
      \brief Status of current calculation
    */
    bool calculationStarted;

    /** 
      \brief Maintain the state machine
    */
    enum CalcState { START, DIGIT, OPERATOR, RESULT, SAVE };
    CalcState calcState;
   
    /**
      \brief Key maps for mapping the translations to the values recognized by calc engine
    */
    KeyMap numeralMap;
    KeyMap operationMap;

    /**
      \brief Interface to the core
    */
    CoreInterface* core;

    /**
      \brief single instance for using utility functions
    */
    CalcUtilities* utility;

    /**
      \brief Boolean which says whether the limit banner is displayed or not, used for handling timeout for notifications
    */
    bool exceedingLimitBannerDisplayed;

    /**
      \brief \a operation stores operations during chaining e.g. 1+2*3, when * is pressed we need to save it in \a operation for future use
    */
    QString operation;

    /**
      \brief
    */
    QStringList storedReference;
    QString     lastResult;
    /**
      \brief stores the last valid result, this value is used for saving into history
    */
    QString     lastSavedResult;
    /**
      \brief a item when written to history becomes a \a savedCandidate
    */
    CalculationItem saveCandidate;
    CalculationItem saveCandidateFromHistory;
    
    SavedCalculations*  savedCalculations;
    CalculationHistory* calculationHistory;
    
    MDialog* saveDialog;
    MTextEdit* saveText;
    int saveCalculationN;

    bool saveCalculationFromHistory;
    bool saveInProgress;
};

#endif // BASICCALC_H
