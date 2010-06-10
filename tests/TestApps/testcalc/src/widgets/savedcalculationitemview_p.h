/* * This file is part of calc application *
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Alvaro Manera <alvaro.manera@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */
#ifndef SAVEDCALCULATIONITEMVIEWVIEW_P_H
#define SAVEDCALCULATIONITEMVIEW_P_H

#include <iostream>
#include <sstream>

#include <QObject>

#define MAX_VALUE_LENGTH 15

#include "savedcalculationitemstyle.h"
#include "calcutilities.h"

class QGraphicsLinearLayout;
class SavedCalculationItem;
class SavedCalculationItemView;
class MLabel;
class MImage;
class MButton;
class MScalableImage;


class SavedCalculationItemViewPrivate : public QObject
{
	Q_OBJECT

 public:
	SavedCalculationItemViewPrivate(SavedCalculationItemView *p);

	~SavedCalculationItemViewPrivate();

	void initBackgroundFunctions();

	void organizeSavedCalculationItemLayout();
	void updateText();
	/*!
	 * \brief Mouse state on SavedCalculationItem Widget
	 */
	enum SavedCalculationItemState
	{
		PressedState,
		ReleasedState
	};

	SavedCalculationItem *controller;
	SavedCalculationItemState savedcalculationitemState;

	int width;
	int height;
	int rotation;
	bool vertical;

	//Widgets

	QGraphicsLinearLayout *vbox;
	QGraphicsLinearLayout *hbox;

	MLabel *labelName;
	MLabel *labelTimeOfCalculation;
	MLabel *labelResult;
	MLabel *labelInput;
	
        typedef const MScalableImage* (SavedCalculationItemStyle::*backgroundFunc)() const;
	static QVector<backgroundFunc> backgroundFunctions;


 private:
	CalcUtilities* utility;
	
	SavedCalculationItemView *q_ptr;
	Q_DECLARE_PUBLIC(SavedCalculationItemView);

};

#endif
