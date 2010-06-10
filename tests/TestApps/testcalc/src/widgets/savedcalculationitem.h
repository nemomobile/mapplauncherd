/* * This file is part of calc application *
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Abhijit Apte<abhijit.apte@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 */

#ifndef SAVEDCALCULATIONITEM_H
#define SAVEDCALCULATIONITEM_H

#include <mwidgetcontroller.h>

#include "savedcalculationitemmodel.h"

//class SavedCalculationItemPrivate;

/*!
 * \class SavedCalculationItem
 * \brief SavedCalculationItem widget sends a changed signal on release
 *
 * SavedCalculationItem sets up a model with two members: iconID and text. IconID
 * contains the ID of the icon drawn on the alarm by the view and
 * text is the alarm label. The details of how these get drawn is
 * dictated by the view
 */
class M_EXPORT SavedCalculationItem : public MWidgetController
{
	Q_OBJECT
	M_CONTROLLER(SavedCalculationItem)

	public:

	enum ContentItemStyle
	{
	IconAndTwoTextLabels = 0,
	SingleTextLabel,
	IconAndSingleTextLabel,
	TwoTextLabels,
	SingleIcon,
	IconAndSingleTextLabelVertical,
	IconAndTwoTextLabelsVertical
	};

	enum ContentItemMode
	{
		Default = 0,
		Single,
		SingleColumnTop,
		SingleColumnCenter,
		SingleColumnBottom
	};

	Q_PROPERTY(ContentItemStyle itemStyle READ itemStyle)
	Q_PROPERTY(ContentItemMode itemMode READ itemMode WRITE setItemMode)

	SavedCalculationItem::ContentItemMode itemMode() const;
	SavedCalculationItem::ContentItemStyle itemStyle() const;
	void setItemMode(ContentItemMode mode);

	/*!
	 * \brief Constructs a alarm with default type
	 * \param parent Parent widget
	 */
	SavedCalculationItem(MWidget *parent = 0);

	~SavedCalculationItem();


	/*!
	  \brief This method emits the clicked() signal
	*/
	void emitClickedSignal();

	/*!
	  \brief This method emits the alarmStateChanged() signal
	*/
	void emitSavedCalculationItemStateChanged();

	void setTimeOfCalculation( const QString& );
	void setResult( const QString& );
	void setInput( const QString& );
	void setName( const QString& );
	void setIndex( const QString& );

	const QString& timeOfCalculation();
	const QString& result();
	const QString& input();
	const QString& name();
	const QString& index();

 signals:
	/*!
	 * \brief A signal that is emitted when the alarm is released
	 */
	void clicked(void);

	/*!
	 * \brief signal emitted when SavedCalculationItem enable/disable button is toggled
	 */
	void savedcalculationitemStateChanged(void);

 private:

	//SavedCalculationItemPrivate *d_ptr;
	Q_DISABLE_COPY(SavedCalculationItem)
};

#endif
