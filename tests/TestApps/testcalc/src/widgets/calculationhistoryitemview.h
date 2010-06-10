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

#ifndef CALCULATIONHISTORYITEMVIEW_H
#define CALCULATIONHISTORYITEMVIEW_H

#include <MWidgetView>
#include <MLocale>
#include <MCalendar>

#include "calculationhistoryitemmodel.h"
#include "calculationhistoryitemstyle.h"

#define MAX_VALUE_LENGTH 15

class CalculationHistoryItemViewPrivate;
class CalculationHistoryItem;

class QGraphicsSceneResizeEvent;

/*!
 * \class CalculationHistoryItemView
 * \brief CalculationHistoryItemView implements a view for the calculationhistoryitem widget
 *
 * CalculationHistoryItemView draws a fixed background and icon and text
 * on top of it. Pressing he bell button will set/reset the calculationhistoryitem.
 */
class M_EXPORT CalculationHistoryItemView : public MWidgetView
{
	Q_OBJECT
	M_VIEW(CalculationHistoryItemModel, CalculationHistoryItemStyle)
	friend class CalculationHistoryItemViewPrivate;

public:
	/*!
	 * \brief Constructor
	 * \param controller Pointer to the calculationhistoryitem's controller
	 */
	CalculationHistoryItemView(CalculationHistoryItem *controller);

	virtual ~CalculationHistoryItemView();

	virtual void resizeEvent(QGraphicsSceneResizeEvent * event);

	virtual QRectF boundingRect () const;
	virtual void updateSize();
	virtual QPainterPath shape() const;
	virtual void resize(qreal w, qreal h);

 protected:
	//! \reimp
	//virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF & constraint = QSizeF()) const;
	virtual void drawBackground(QPainter* painter, const QStyleOptionGraphicsItem* option) const;

	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	void setSelected(bool selected);

	virtual void applyStyle();
	virtual void setupModel();
	//! \reimp_end

 protected slots:
	virtual void updateData(const QList<const char*>& modifications);

 signals:
	/*!
	 * \brief Signals that the clock has been released
	 */
	void clicked(void);

 private:
	Q_DISABLE_COPY(CalculationHistoryItemView)

	CalculationHistoryItemViewPrivate *d_ptr;

	void organizeCalculationHistoryItemLayout();

};

#endif
