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

#ifndef CALCULATIONHISTORYITEM_H
#define CALCULATIONHISTORYITEM_H

#include <mwidgetcontroller.h>

#include "calculationhistoryitemmodel.h"

//class CalculationHistoryItemPrivate;

/*!
 * \class CalculationHistoryItem
 * \brief CalculationHistoryItem widget sends a changed signal on release
 *
 * CalculationHistoryItem sets up a model with two members: iconID and text. IconID
 * contains the ID of the icon drawn on the alarm by the view and
 * text is the alarm label. The details of how these get drawn is
 * dictated by the view
 */
class M_EXPORT CalculationHistoryItem : public MWidgetController
{
	Q_OBJECT
	M_CONTROLLER(CalculationHistoryItem)

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


	/*!
	 * \brief Constructs a alarm with default type
	 * \param parent Parent widget
	 */
	CalculationHistoryItem(MWidget *parent = 0);

	~CalculationHistoryItem();

	CalculationHistoryItem::ContentItemMode itemMode() const;
	CalculationHistoryItem::ContentItemStyle itemStyle() const;
	void setItemMode(ContentItemMode mode);


	/*!
	  \brief This method emits the clicked() signal
	*/
	void emitClickedSignal();

	void emitCalculationHistoryItemStateChanged();

	void setTimeOfCalculation( const QString& );
	void setResult( const QString& );
	void setInput( const QString& );
	void setIndex( const QString& );
	void setElide( Qt::TextElideMode );

	const QString& timeOfCalculation();
	const QString& result();
	const QString& input();
	const QString& index();
	const Qt::TextElideMode elide();

	const bool selected();

	QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value);

 signals:
	/*!
	 * \brief A signal that is emitted when the alarm is released
	 */
	void clicked(void);

	/*!
	 * \brief signal emitted when CalculationHistoryItem enable/disable button is toggled
	 */
	void calculationhistoryitemStateChanged(void);

 private:

	//CalculationHistoryItemPrivate *d_ptr;
	Q_DISABLE_COPY(CalculationHistoryItem)
};

#endif
