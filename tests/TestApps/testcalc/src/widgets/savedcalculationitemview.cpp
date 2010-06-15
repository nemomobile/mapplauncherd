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

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QDebug>

//#include <mlinearlayout.h>
#include <MLabel>
#include <MButton>
#include <MScalableImage>

#include "savedcalculationitemview.h"
#include "savedcalculationitemview_p.h"
#include "savedcalculationitem.h"

QVector<SavedCalculationItemViewPrivate::backgroundFunc> SavedCalculationItemViewPrivate::backgroundFunctions;

SavedCalculationItemViewPrivate::SavedCalculationItemViewPrivate(SavedCalculationItemView *p)
				: controller(0),
				  savedcalculationitemState(ReleasedState),
				  vertical(FALSE),
				  hbox(NULL),
				  vbox(NULL),
				  labelTimeOfCalculation(NULL),
				  labelName(NULL),
				  labelResult(NULL),
				  labelInput(NULL),
				  q_ptr(p)

{
    utility = CalcUtilities::instance();
    hbox = new QGraphicsLinearLayout(Qt::Horizontal);
    vbox = new QGraphicsLinearLayout(Qt::Vertical);
    initBackgroundFunctions();
}

SavedCalculationItemViewPrivate::~SavedCalculationItemViewPrivate()
{}

void SavedCalculationItemViewPrivate::initBackgroundFunctions()
{
    if(backgroundFunctions.size() == 0)
    {
        backgroundFunctions.resize(SavedCalculationItem::SingleColumnBottom + 1);
        backgroundFunctions[SavedCalculationItem::Single] = &SavedCalculationItemStyle::backgroundImageSingle;
        backgroundFunctions[SavedCalculationItem::SingleColumnTop] = &SavedCalculationItemStyle::backgroundImageSinglecolumnTop;
        backgroundFunctions[SavedCalculationItem::SingleColumnCenter] = &SavedCalculationItemStyle::backgroundImageSinglecolumnCenter;
        backgroundFunctions[SavedCalculationItem::SingleColumnBottom] = &SavedCalculationItemStyle::backgroundImageSinglecolumnBottom;
    }
}

void SavedCalculationItemViewPrivate::organizeSavedCalculationItemLayout()
{
    vbox->setSpacing( 0 );
    
    labelName = new MLabel();
    labelName->setObjectName( "historyName" );
    labelName->setTextElide( true );

    vbox->addItem( labelName );
    
    labelResult = new MLabel();
    labelResult->setObjectName( "historyStoredRes" );
    hbox->addItem( labelResult );

    labelTimeOfCalculation = new MLabel();
    labelTimeOfCalculation->setObjectName( "historyTimeOfCalc" );
    labelTimeOfCalculation->setAlignment( Qt::AlignRight );
    hbox->addItem( labelTimeOfCalculation );
    vbox->addItem( hbox );

    labelInput = new MLabel();
    labelInput->setObjectName( "historyStoredRef" );
    labelInput->setTextElide( true );
    // TODO: temporarily the next line has been disabled.
    // This again has to be re-enabled after bug# 146158
    // has been fixed.
    // labelInput->setTextElide( Qt::ElideRight );
    vbox->addItem( labelInput );

    controller->setLayout( vbox );
}

void SavedCalculationItemViewPrivate::updateText()
{
    labelName->setText( q_ptr->model()->name() );

    MCalendar calendar;
    MLocale locale;
    
    QDateTime qdt = QDateTime::fromString( q_ptr->model()->timeOfCalculation(), "hh:mm ap MMM d, yyyy" );
    QDateTime curDateTime = QDateTime::currentDateTime();
    
    qdt.setTimeSpec( Qt::LocalTime );
    calendar.setDateTime( qdt );
    
    QString mystring;

    if( qdt.daysTo( curDateTime ) == 0 )
    {
	mystring = locale.formatDateTime( calendar, MLocale::DateNone, MLocale::TimeShort );
    }
    else
    {
	mystring = locale.formatDateTime( calendar, MLocale::DateMedium, MLocale::TimeShort );
    }
    
    labelTimeOfCalculation->setText(mystring);
    //labelTimeOfCalculation->setText(q_ptr->model()->timeOfCalculation());
    QString numeral = q_ptr->model()->result();
    if( numeral.length() > MAX_VALUE_LENGTH )
	utility->convertToExponential( numeral );

    if( numeral.contains("e") )
    {
	QStringList splitText = numeral.split(  QChar('e') );
	qDebug() << "splitText[0]=" << splitText[0] << endl;
	qDebug() << "splitText[1]=" << splitText[1] << endl;
	mystring = locale.formatNumber( splitText[0].toDouble(), MAX_VALUE_LENGTH ) 
			+ "e" ;
	if( splitText[1].toDouble() > 0.0 )
	    mystring += QString("+");
	mystring += locale.formatNumber( splitText[1].toDouble(), MAX_VALUE_LENGTH );
    }
    else if( numeral.contains("E") )
    {
	QStringList splitText = numeral.split(  QChar('E') );
	qDebug() << "splitText[0]=" << splitText[0] << endl;
	qDebug() << "splitText[1]=" << splitText[1] << endl;
	mystring = locale.formatNumber( splitText[0].toDouble(), MAX_VALUE_LENGTH ) 
			+ "E" ;
	if( splitText[1].toDouble() > 0.0 )
	    mystring += QString("+");
	mystring += locale.formatNumber( splitText[1].toDouble(), MAX_VALUE_LENGTH );
    }
    else
	mystring = locale.formatNumber( numeral.toDouble(), MAX_VALUE_LENGTH );

    QTextStream debugStream(stderr);
    debugStream.setCodec("UTF-8");
    debugStream << "localized number =" << mystring << endl;

    labelResult->setText( mystring );
    
    // 
    // LOGIC FOR LOCALIZING INPUT STRING 
    // This logic takes care of localizing the input string according to the display language setting
    //
    QString num = q_ptr->model()->input();
    QRegExp rx("^?[1-9]+\\.?\\d*$?");
    int idx=rx.indexIn( num);
    int pos=0;
    qDebug() << "my reg exp matches " << rx.captureCount() << " times" << endl;
    while(rx.indexIn(num, pos) != -1)
    {
	idx=rx.indexIn( num,pos);
        num.replace(rx.cap(0),locale.formatNumber(rx.cap(0).toDouble(),MAX_VALUE_LENGTH ));
        pos += rx.matchedLength();
    }
    labelInput->setText(num);
    labelInput->setTextElide( true );
}

SavedCalculationItemView::SavedCalculationItemView(SavedCalculationItem *controller)
		 : MWidgetView(controller),
		   d_ptr(new SavedCalculationItemViewPrivate(this))
{
	d_ptr->controller = controller;
	d_ptr->width = 0;
	d_ptr->height = 0;
	d_ptr->organizeSavedCalculationItemLayout();

	update();
}

SavedCalculationItemView::~SavedCalculationItemView()
{
	delete d_ptr;
	d_ptr = 0;
}

void SavedCalculationItemView::resizeEvent(QGraphicsSceneResizeEvent * event)
{
	Q_UNUSED( event );

	updateSize();
}

void SavedCalculationItemView::drawBackground(QPainter* painter, const QStyleOptionGraphicsItem* option) const
{
    Q_UNUSED(option);
    SavedCalculationItemViewPrivate::backgroundFunc func = SavedCalculationItemViewPrivate::backgroundFunctions[model()->itemMode()];
    const SavedCalculationItemStyle * itemStyle = style().operator ->();
    const MScalableImage* itemBackground = ((*itemStyle).*func)();
    itemBackground->draw(0, 0, size().width(), size().height(), painter);
}

QRectF SavedCalculationItemView::boundingRect() const
{
	return QRectF(QPointF(), d_ptr->controller->size());
}

QPainterPath SavedCalculationItemView::shape() const
{
	QPainterPath path;
	path.addRect(QRectF(QPointF(), d_ptr->controller->size()));
	return path;
}

void SavedCalculationItemView::resize(qreal w, qreal h)
{
	Q_UNUSED( w );
	Q_UNUSED( h );

	updateSize();
}

void SavedCalculationItemView::updateSize()
{
	QSizeF prefSize = sizeHint(Qt::MinimumSize, QSizeF());

	int width = d_ptr->controller->size().width();
	int height = d_ptr->controller->size().height();

	if (width <= 0)
		width = prefSize.width();

	if (height <= 0)
		height = prefSize.height();

	if (width > 0 && height > 0 && (d_ptr->width != width || d_ptr->height != height))
	{
		d_ptr->width = width;
		d_ptr->height = height;
	}
}

void SavedCalculationItemView::applyStyle()
{
	updateSize();

	MWidgetView::applyStyle();

}

void SavedCalculationItemView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);

	if (d_ptr->savedcalculationitemState == SavedCalculationItemViewPrivate::PressedState)
		return;

	d_ptr->savedcalculationitemState = SavedCalculationItemViewPrivate::PressedState;
}

void SavedCalculationItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);

	if (d_ptr->savedcalculationitemState == SavedCalculationItemViewPrivate::ReleasedState)
		return;

	d_ptr->savedcalculationitemState = SavedCalculationItemViewPrivate::ReleasedState;

	// bounding box collision detection
	d_ptr->controller->emitClickedSignal();
}

void SavedCalculationItemView::setupModel()
{
	MWidgetView::setupModel();

	d_ptr->updateText();

	update();
}

void SavedCalculationItemView::updateData(const QList<const char*>& modifications)
{
	MWidgetView::updateData(modifications);

	d_ptr->updateText();

	update();
}

M_REGISTER_VIEW_NEW(SavedCalculationItemView, SavedCalculationItem)
