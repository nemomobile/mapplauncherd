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

#ifndef CALCULATIONLINEWIDGETITEM_H
#define CALCULATIONLINEWIDGETITEM_H

#include <mwidgetcontroller.h>

#include "calculationlinewidgetmodel.h"

//class CalculationLineWidgetPrivate;

/*!
 * \class CalculationLineWidget
 * \brief CalculationLineWidget widget sends a changed signal on release
 *
 * CalculationLineWidget sets up a model with two members: iconID and text. IconID
 * contains the ID of the icon drawn on the alarm by the view and
 * text is the alarm label. The details of how these get drawn is
 * dictated by the view
 */
class M_EXPORT CalculationLineWidget : public MWidgetController
{
	Q_OBJECT
	M_CONTROLLER(CalculationLineWidget)

	public:

	enum ContentItemMode
	{
	Default = 0,
	Single
	};

	Q_PROPERTY(ContentItemMode itemMode READ itemMode WRITE setItemMode)

	/*!
	 * \brief Constructs a alarm with default type
	 * \param parent Parent widget
	 */
	CalculationLineWidget(MWidget *parent = 0);

	~CalculationLineWidget();

	/*!
	  \brief This method emits the clicked() signal
	*/
	void emitClickedSignal();

	void emitCalculationLineWidgetStateChanged();

	void setText( const QString& );

	const QString& text();

	CalculationLineWidget::ContentItemMode itemMode() const;
	void setItemMode(ContentItemMode mode);

	QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value);

 signals:
	/*!
	 * \brief A signal that is emitted when the alarm is released
	 */
	void clicked(void);

	/*!
	 * \brief signal emitted when CalculationLineWidget enable/disable button is toggled
	 */
	void calculationlinewidgetStateChanged(void);

 private:

	//CalculationLineWidgetPrivate *d_ptr;
	Q_DISABLE_COPY(CalculationLineWidget)
};

#endif
