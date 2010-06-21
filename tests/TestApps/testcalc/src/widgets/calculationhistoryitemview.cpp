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

#include <iostream>
#include <sstream>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsLinearLayout>
#include <QDebug>
#include <QTextStream>

//#include <mlinearlayout.h>
#include <MLabel>
#include <MButton>
#include <MScalableImage>
#include <MApplication>

#include "calculationhistoryitemview.h"
#include "calculationhistoryitemview_p.h"
#include "calculationhistoryitem.h"
#include <calcconstants.h>
using namespace std;

QVector<CalculationHistoryItemViewPrivate::backgroundFunc> CalculationHistoryItemViewPrivate::backgroundFunctions;

CalculationHistoryItemViewPrivate::CalculationHistoryItemViewPrivate(CalculationHistoryItemView *p)
				: controller(0),
				  calculationhistoryitemState(ReleasedState),
				  vertical(FALSE),
				  vbox(NULL),
				  hbox( NULL ),
				  labelTimeOfCalculation(NULL),
				  labelResult(NULL),
				  labelInput(NULL),
				  q_ptr(p)

{
    utility = CalcUtilities::instance();
    vbox = new QGraphicsLinearLayout( Qt::Vertical );
    hbox = new QGraphicsLinearLayout( Qt::Horizontal );
    initBackgroundFunctions();
}

CalculationHistoryItemViewPrivate::~CalculationHistoryItemViewPrivate()
{}

void CalculationHistoryItemViewPrivate::initBackgroundFunctions()
{
    if(backgroundFunctions.size() == 0)
    {
	backgroundFunctions.resize(CalculationHistoryItem::SingleColumnBottom + 1);
	backgroundFunctions[CalculationHistoryItem::Single] = &CalculationHistoryItemStyle::backgroundImageSingle;
	backgroundFunctions[CalculationHistoryItem::SingleColumnTop] = &CalculationHistoryItemStyle::backgroundImageSinglecolumnTop;
	backgroundFunctions[CalculationHistoryItem::SingleColumnCenter] = &CalculationHistoryItemStyle::backgroundImageSinglecolumnCenter;
	backgroundFunctions[CalculationHistoryItem::SingleColumnBottom] = &CalculationHistoryItemStyle::backgroundImageSinglecolumnBottom;
    }
}

void CalculationHistoryItemViewPrivate::organizeCalculationHistoryItemLayout()
{
    vbox->setSpacing( 0 );
    hbox->setSpacing( 0 );

    labelResult = new MLabel();
    labelResult->setObjectName( "historyStoredRes" );
    hbox->addItem( labelResult );

    labelTimeOfCalculation = new MLabel();
    labelTimeOfCalculation->setObjectName( "historyTimeOfCalc" );
    labelTimeOfCalculation->setAlignment( Qt::AlignRight );
    hbox->addItem( labelTimeOfCalculation );

    labelInput = new MLabel();
    labelInput->setObjectName( "historyStoredRef" );
    labelInput->setTextElide( true );
	
   /*	labelError = new MLabel();
	labelError->setObjectName( "historyError" );
	hbox1->addItem(labelError);
	hbox1->addItem(labelInput);*/
	
    // TODO: temporarily the next line has been disabled.
    // This again has to be re-enabled after bug# 146158
    // has been fixed.
    //labelInput->setTextElide( q_ptr->model()->elide() );
    vbox->addItem( hbox );
    vbox->addItem( labelInput );

    controller->setLayout( vbox );
}

void CalculationHistoryItemViewPrivate::updateText()
{
    MCalendar calendar;
    MLocale locale;

    QDateTime qdt = QDateTime::fromString( q_ptr->model()->timeOfCalculation(), "hh:mm ap MMM d, yyyy" );
    QDateTime curDateTime = QDateTime::currentDateTime();
    
    qdt.setTimeSpec( Qt::LocalTime );
    calendar.setDateTime( qdt );

    QString mystring;
    
    if( qdt.daysTo( curDateTime ) == 0 )
    {
	// display just the time
	mystring = locale.formatDateTime( calendar, MLocale::DateNone, MLocale::TimeShort );
    }
    else
    {
        // just the date
        //mystring = locale.formatDateTime( calendar, MLocale::DateMedium );
        mystring = locale.formatDateTime( calendar, MLocale::DateMedium, MLocale::TimeShort );
    }
    
    qDebug() << "mystring = " << mystring << endl;
    labelTimeOfCalculation->setText(mystring);
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
    QRegExp rx("[1-9]+\\.?\\d*$?");
    int idx=rx.indexIn( num);
    int pos=0;
    qDebug() << "my reg exp matches " << rx.captureCount() << " times" << endl;
	qDebug() << "BEFORE REPLACE "<<num<<endl;
    while(rx.indexIn(num, pos) != -1)
    {
		idx=rx.indexIn( num,pos);
        num.replace(rx.cap(0),locale.formatNumber(rx.cap(0).toDouble(),MAX_VALUE_LENGTH ));
        pos += rx.matchedLength();
    }
	qDebug() << "AFTER REPLACE "<<num<<endl;
/*	if((num.contains(TXT_CALC_NOTE_CALC_LIMIT)||num.contains(TXT_CALC_NOTE_INDICATION)))				
	{
	  if(num.contains(TXT_CALC_NOTE_CALC_LIMIT))
	  {
	 	num.remove(TXT_CALC_NOTE_CALC_LIMIT); 
		labelError->setText(TXT_CALC_NOTE_CALC_LIMIT);
	  }
	  else 
	  {
			num.remove(TXT_CALC_NOTE_INDICATION);
			labelError->setText(TXT_CALC_NOTE_INDICATION);
	  }
	  labelError->setColor(Qt::red);
	}
	else
	{
		labelError->setText("");
	}*/
			
	labelInput->setText(num);
    labelInput->setTextElide( true );
}

CalculationHistoryItemView::CalculationHistoryItemView(CalculationHistoryItem *controller)
		 : MWidgetView(controller),
		   d_ptr(new CalculationHistoryItemViewPrivate(this))
{
	d_ptr->controller = controller;
	d_ptr->width = 0;
	d_ptr->height = 0;
	d_ptr->organizeCalculationHistoryItemLayout();

	update();
}

CalculationHistoryItemView::~CalculationHistoryItemView()
{
	delete d_ptr;
	d_ptr = 0;
}

void CalculationHistoryItemView::resizeEvent(QGraphicsSceneResizeEvent * event)
{
	Q_UNUSED( event );

	updateSize();
}

void CalculationHistoryItemView::drawBackground(QPainter* painter, const QStyleOptionGraphicsItem* option) const
{
    Q_UNUSED(option);
    
    CalculationHistoryItemViewPrivate::backgroundFunc func = CalculationHistoryItemViewPrivate::backgroundFunctions[model()->itemMode()];
    const CalculationHistoryItemStyle * itemStyle = style().operator ->();
    const MScalableImage* itemBackground = ((*itemStyle).*func)();
    itemBackground->draw(0, 0, size().width(), size().height(), painter);
}

QRectF CalculationHistoryItemView::boundingRect() const
{
    return QRectF(QPointF(), d_ptr->controller->size());
}

QPainterPath CalculationHistoryItemView::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(QPointF(), d_ptr->controller->size()));
    return path;
}

void CalculationHistoryItemView::resize(qreal w, qreal h)
{
    Q_UNUSED( w );
    Q_UNUSED( h );
    
    updateSize();
}

void CalculationHistoryItemView::updateSize()
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

void CalculationHistoryItemView::applyStyle()
{
	updateSize();

	MWidgetView::applyStyle();

}

void CalculationHistoryItemView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);

	if (d_ptr->calculationhistoryitemState == CalculationHistoryItemViewPrivate::PressedState)
		return;

	d_ptr->calculationhistoryitemState = CalculationHistoryItemViewPrivate::PressedState;
}

void CalculationHistoryItemView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);
	qDebug() << Q_FUNC_INFO << endl;

	if (d_ptr->calculationhistoryitemState == CalculationHistoryItemViewPrivate::ReleasedState)
		return;

	d_ptr->calculationhistoryitemState = CalculationHistoryItemViewPrivate::ReleasedState;

	// bounding box collision detection
	d_ptr->controller->emitClickedSignal();
}

void CalculationHistoryItemView::setupModel()
{
	MWidgetView::setupModel();
	qDebug() << Q_FUNC_INFO << endl;

	d_ptr->updateText();

	update();
}

void CalculationHistoryItemView::updateData(const QList<const char*>& modifications)
{
	MWidgetView::updateData(modifications);
	qDebug() << Q_FUNC_INFO << endl;

	d_ptr->updateText();

	update();

	const char* member;
	for (int i=0; i<modifications.count(); i++) {
	    member = modifications[i];
	    if(member == CalculationHistoryItemModel::Selected){
		setSelected(model()->selected());
	    }
	}
}

void CalculationHistoryItemView::setSelected(bool selected)
{
    if(selected)
        style().setModeSelected();
    else
        style().setModeDefault();
    applyStyle();
    updateGeometry();
}


M_REGISTER_VIEW_NEW(CalculationHistoryItemView, CalculationHistoryItem)
