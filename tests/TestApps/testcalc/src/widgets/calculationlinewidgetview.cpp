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
#include <MApplication>

#include "calculationlinewidgetview.h"
#include "calculationlinewidgetview_p.h"
#include "calculationlinewidget.h"

QVector<CalculationLineWidgetViewPrivate::backgroundFunc> CalculationLineWidgetViewPrivate::backgroundFunctions;

CalculationLineWidgetViewPrivate::CalculationLineWidgetViewPrivate(CalculationLineWidgetView *p)
				: controller(0),
				  calculationlinewidgetState(ReleasedState),
				  vertical(FALSE),
				  vbox( NULL ),
				  calculationLine(NULL),
				  q_ptr(p)

{
    vbox = new QGraphicsLinearLayout( Qt::Vertical );
    initBackgroundFunctions();
}

CalculationLineWidgetViewPrivate::~CalculationLineWidgetViewPrivate()
{}

void CalculationLineWidgetViewPrivate::initBackgroundFunctions()
{
    if(backgroundFunctions.size() == 0)
    {
	backgroundFunctions.resize(CalculationLineWidget::Single+ 1);
	backgroundFunctions[CalculationLineWidget::Single] = &CalculationLineWidgetStyle::backgroundImageSingle;
    }
}

void CalculationLineWidgetViewPrivate::organizeCalculationLineWidgetLayout()
{
    vbox->setSpacing( 0 );
    calculationLine = new MLabel("0");
    calculationLine->setAlignment( Qt::AlignRight );
    calculationLine->setObjectName( "calculationLineItem" );
    vbox->addItem( calculationLine );

    controller->setLayout( vbox );
}

void CalculationLineWidgetViewPrivate::updateText()
{
    MLocale locale;
    //calculationLine->setText(q_ptr->model()->text());
    QString numeral = q_ptr->model()->text();
    if( numeral.length() > MAX_VALUE_LENGTH )
	this->convertToExponential( numeral );
    
    QString mystring;
    if( numeral.contains("e") )
    { // exponential
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
    { // exponential
	QStringList splitText = numeral.split(  QChar('E') );
	qDebug() << "splitText[0]=" << splitText[0] << endl;
	qDebug() << "splitText[1]=" << splitText[1] << endl;
	mystring = locale.formatNumber( splitText[0].toDouble(), MAX_VALUE_LENGTH ) 
		   + "E" ;
	if( splitText[1].toDouble() > 0.0 )
	    mystring += QString("+");
	mystring += locale.formatNumber( splitText[1].toDouble(), MAX_VALUE_LENGTH );
    }
    else if( ( numeral.contains( "." ) &&  numeral.endsWith( "0" ) ) || 
	     numeral.endsWith( "." ) )
    { // non-exponential
	mystring = locale.formatNumber( numeral.toDouble(), MAX_VALUE_LENGTH );
	if( !mystring.contains( QString(locale.formatNumber(1.2)[1]) ) )
	    mystring += QString(locale.formatNumber(1.2)[1]);
	if( numeral.endsWith("0" ) )
	{
	    // LOGIC FOR LEADING ZEROES IN DECIMAL POINT
	    QRegExp rx( "\\.0*$" );
	    int idx = rx.indexIn( numeral, 0 );
	    int len1 = rx.matchedLength();
	    qDebug() << "matched length for rx( \"\\\\.0*\" ) = " << len1 << endl;
	    for( int i=0; i < len1 - 1; i++ )
		mystring += QString( locale.formatNumber( 0 ) );

	    // LOGIC FOR TRAILING ZEROES IN DECIMAL POINT
	    rx.setPattern( "^\\-?[0-9]+\\.[0-9]*[1-9]+0*$" );
	    idx = rx.indexIn( numeral, 0 );
	    len1 = rx.matchedLength();
	    qDebug() << "len = " << len1 << endl;
	    rx.setPattern( "^\\-?[0-9]+\\.[0-9]*[1-9]+" );
	    idx = rx.indexIn( numeral, 0 );
	    int len2 = rx.matchedLength();
	    for( int i=0; i < len1-len2; i++ )
		mystring += QString( locale.formatNumber( 0 ) );
	}
    }
    else
    { // non-exponential
        mystring = locale.formatNumber( numeral.toDouble(), MAX_VALUE_LENGTH );
    }
    QTextStream debugStream(stderr);
    debugStream.setCodec("UTF-8");
    debugStream << "localized number =" << mystring << endl;

//    calculationLine->setText( numeral );
    calculationLine->setText( mystring );
}

CalculationLineWidgetView::CalculationLineWidgetView(CalculationLineWidget *controller)
		 : MWidgetView(controller),
		   d_ptr(new CalculationLineWidgetViewPrivate(this))
{
	d_ptr->controller = controller;
	d_ptr->width = 0;
	d_ptr->height = 0;
	d_ptr->organizeCalculationLineWidgetLayout();

	update();
}

CalculationLineWidgetView::~CalculationLineWidgetView()
{
	delete d_ptr;
	d_ptr = 0;
}

void CalculationLineWidgetView::resizeEvent(QGraphicsSceneResizeEvent * event)
{
	Q_UNUSED( event );

	updateSize();
}

void CalculationLineWidgetView::drawBackground(QPainter* painter, const QStyleOptionGraphicsItem* option) const
{
    Q_UNUSED(option);
    
    CalculationLineWidgetViewPrivate::backgroundFunc func = CalculationLineWidgetViewPrivate::backgroundFunctions[model()->itemMode()];
    const CalculationLineWidgetStyle * itemStyle = style().operator ->();
    const MScalableImage* itemBackground = ((*itemStyle).*func)();
    itemBackground->draw(0, 0, size().width(), size().height(), painter);
}

QRectF CalculationLineWidgetView::boundingRect() const
{
    return QRectF(QPointF(), d_ptr->controller->size());
}

QPainterPath CalculationLineWidgetView::shape() const
{
    QPainterPath path;
    path.addRect(QRectF(QPointF(), d_ptr->controller->size()));
    return path;
}

void CalculationLineWidgetView::resize(qreal w, qreal h)
{
    Q_UNUSED( w );
    Q_UNUSED( h );
    
    updateSize();
}

void CalculationLineWidgetView::updateSize()
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

void CalculationLineWidgetView::applyStyle()
{
	updateSize();

	MWidgetView::applyStyle();

}

void CalculationLineWidgetView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);

	if (d_ptr->calculationlinewidgetState == CalculationLineWidgetViewPrivate::PressedState)
		return;

	d_ptr->calculationlinewidgetState = CalculationLineWidgetViewPrivate::PressedState;
}

void CalculationLineWidgetView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);
	qDebug() << Q_FUNC_INFO << endl;

	if (d_ptr->calculationlinewidgetState == CalculationLineWidgetViewPrivate::ReleasedState)
		return;

	d_ptr->calculationlinewidgetState = CalculationLineWidgetViewPrivate::ReleasedState;

	// bounding box collision detection
	d_ptr->controller->emitClickedSignal();
}

void CalculationLineWidgetView::setupModel()
{
	MWidgetView::setupModel();
	qDebug() << Q_FUNC_INFO << endl;

	d_ptr->updateText();

	update();
}

void CalculationLineWidgetView::updateData(const QList<const char*>& modifications)
{
	MWidgetView::updateData(modifications);
	qDebug() << Q_FUNC_INFO << endl;

	d_ptr->updateText();

	update();

	const char* member;
	for (int i=0; i<modifications.count(); i++) {
	    member = modifications[i];
	    if(member == CalculationLineWidgetModel::Selected){
		setSelected(model()->selected());
	    }
	}
}

void CalculationLineWidgetView::setSelected(bool selected)
{
    if(selected)
        style().setModeSelected();
    else
        style().setModeDefault();
    applyStyle();
    updateGeometry();
}

void CalculationLineWidgetViewPrivate::convertToExponential( QString& numeral )
{
    QString backup = numeral;
    
    if( (numeral.startsWith( "-" )) && 
	   ( (!numeral.contains( "." ) && numeral.length() <= MAX_VALUE_LENGTH + 1) ||
	     (numeral.contains( "." ) && numeral.length() <= MAX_VALUE_LENGTH + 2) ) )
	// include the negative sign also while considering the maximum permissible length in non-scientific format
    {
	qDebug() << "don't convert just return back" << endl;
	return;
    }
    else if( !numeral.startsWith("-") &&
             numeral.contains( "." ) &&
	     numeral.length() <= MAX_VALUE_LENGTH+1 )
	return;

    std::cout.precision( MAX_VALUE_LENGTH );
    std::cout.flags( std::ios_base::scientific );
    std::ostringstream os( std::ostringstream::out );
    os.precision( MAX_VALUE_LENGTH );
    os << numeral.toDouble();
    numeral = os.str().data();

    // Flush the output string stream for future use again
    os.str( "" );

    if( numeral.contains( "e" ) )
    {
	QStringList parts = numeral.split( "e" );
	if( parts[1].toInt() < -MAX_VALUE_LENGTH )
	{
	    numeral = "0";
	    return;
	}
        int choplen = numeral.length() - numeral.indexOf("e");
        std::cout.precision( MAX_VALUE_LENGTH - choplen );
        std::cout.flags( std::ios_base::scientific );
        os.precision( MAX_VALUE_LENGTH - choplen );
        os << numeral.toDouble();
        numeral = os.str().data();
    }
    else if( ( numeral.startsWith( "0." )  && numeral.length() > MAX_VALUE_LENGTH+1) ||
	     ( numeral.startsWith( "-0." )  && numeral.length() > MAX_VALUE_LENGTH+2) )
    {
	// use the backup for truncation/roundoff
	int choplen = numeral.length() - MAX_VALUE_LENGTH + 1;

	// use the regular expression to chop additional numbers if there are leading zeros after 
	// decimal point
	QRegExp rx( "\\.0*" );
	int idx = rx.indexIn( numeral, 0 );
        qDebug() << "idx = " << idx << endl;
	int matchLength = rx.matchedLength();
	choplen += matchLength - 1;
	
	std::cout.precision( MAX_VALUE_LENGTH - choplen );
	os.precision( MAX_VALUE_LENGTH - choplen );
	
	std::cout.precision( numeral.length() - choplen );
	os.precision( numeral.length() - choplen );
	
        std::cout.flags( std::ios_base::scientific );
        os << backup.toDouble();
        numeral = os.str().data();
    }
}

M_REGISTER_VIEW_NEW(CalculationLineWidgetView, CalculationLineWidget)
