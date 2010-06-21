/* * This file is part of calc application *
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
#ifndef CALCULATIONHISTORYITEMVIEWVIEW_P_H
#define CALCULATIONHISTORYITEMVIEW_P_H

#include <QObject>

#include "calculationhistoryitemstyle.h"
#include "calcutilities.h"


class QGraphicsLinearLayout;
class CalculationHistoryItem;
class CalculationHistoryItemView;
//class MLinearLayout;
class MLabel;
class MImage;
class MButton;
class MScalableImage;


class CalculationHistoryItemViewPrivate : public QObject
{
	Q_OBJECT

 public:
	CalculationHistoryItemViewPrivate(CalculationHistoryItemView *p);

	~CalculationHistoryItemViewPrivate();

	static void initBackgroundFunctions();
	void organizeCalculationHistoryItemLayout();
	void updateText();
	/*!
	 * \brief Mouse state on CalculationHistoryItem Widget
	 */
	enum CalculationHistoryItemState
	{
		PressedState,
		ReleasedState
	};

	CalculationHistoryItem *controller;
	CalculationHistoryItemState calculationhistoryitemState;

	int width;
	int height;
	int rotation;
	bool vertical;

	//Widgets

	QGraphicsLinearLayout *vbox;
	QGraphicsLinearLayout *hbox;
	
	MLabel *labelTimeOfCalculation;
	MLabel *labelResult;
	MLabel *labelInput;

	typedef const MScalableImage* (CalculationHistoryItemStyle::*backgroundFunc)() const;
	static QVector<backgroundFunc> backgroundFunctions;

 private:
	CalcUtilities* utility;
	
	CalculationHistoryItemView *q_ptr;
	Q_DECLARE_PUBLIC(CalculationHistoryItemView);

};

#endif
