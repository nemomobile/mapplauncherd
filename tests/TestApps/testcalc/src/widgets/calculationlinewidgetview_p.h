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
#ifndef CALCULATIONLINEWIDGETITEMVIEWVIEW_P_H
#define CALCULATIONLINEWIDGETITEMVIEW_P_H

#include <QObject>

#include "calculationlinewidgetstyle.h"

#include <iostream>
#include <sstream>

#define MAX_VALUE_LENGTH 15

using namespace std;

class QGraphicsLinearLayout;
class CalculationLineWidget;
class CalculationLineWidgetView;
//class MLinearLayout;
class MLabel;
class MImage;
class MButton;
class MScalableImage;


class CalculationLineWidgetViewPrivate : public QObject
{
	Q_OBJECT

 public:
	CalculationLineWidgetViewPrivate(CalculationLineWidgetView *p);

	~CalculationLineWidgetViewPrivate();

	static void initBackgroundFunctions();
	void organizeCalculationLineWidgetLayout();
	void updateText();
	/*!
	 * \brief Mouse state on CalculationLineWidget Widget
	 */
	enum CalculationLineWidgetState
	{
		PressedState,
		ReleasedState
	};

	CalculationLineWidget *controller;
	CalculationLineWidgetState calculationlinewidgetState;

	int width;
	int height;
	int rotation;
	bool vertical;

	//Widgets
	QGraphicsLinearLayout* vbox;
	MLabel *calculationLine;
	
	typedef const MScalableImage* (CalculationLineWidgetStyle::*backgroundFunc)() const;
	static QVector<backgroundFunc> backgroundFunctions;

 private:
	void convertToExponential( QString& );
	CalculationLineWidgetView *q_ptr;
	Q_DECLARE_PUBLIC(CalculationLineWidgetView);

};

#endif

