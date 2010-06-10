#include "calculationhistoryitem.h"
#include "calculationhistoryitemview.h"

CalculationHistoryItem::CalculationHistoryItem(MWidget *parent)
	  //:MWidgetController(parent, new CalculationHistoryItemModel)
	  :MWidgetController(new CalculationHistoryItemModel, parent)
{}

CalculationHistoryItem::~CalculationHistoryItem()
{
}


void CalculationHistoryItem::emitClickedSignal()
{
	emit clicked();
}

void CalculationHistoryItem::emitCalculationHistoryItemStateChanged()
{
	emit calculationhistoryitemStateChanged();
}

void CalculationHistoryItem::setTimeOfCalculation(const QString& timeOfCalc )
{
	model()->setTimeOfCalculation( timeOfCalc);
}

void CalculationHistoryItem::setResult(const QString& result )
{
	model()->setResult( result );
}

void CalculationHistoryItem::setInput(const QString& input )
{
	model()->setInput( input );
}

void CalculationHistoryItem::setIndex( const QString& index )
{
    model()->setIndex( index );
}

void CalculationHistoryItem::setItemMode(ContentItemMode mode)
{
    model()->setItemMode(mode);
}

void CalculationHistoryItem::setElide( Qt::TextElideMode _elide )
{
    model()->setElide( _elide );
}

const QString& CalculationHistoryItem::timeOfCalculation()
{
	return model()->timeOfCalculation();
}

const QString& CalculationHistoryItem::result()
{
	return model()->result();
}

const QString& CalculationHistoryItem::input()
{
	return model()->input();
}

const QString& CalculationHistoryItem::index()
{
    return model()->index();
}

const Qt::TextElideMode CalculationHistoryItem::elide()
{
    return model()->elide();
}

const bool CalculationHistoryItem::selected()
{
    return model()->selected();
}

CalculationHistoryItem::ContentItemStyle CalculationHistoryItem::itemStyle() const
{
	    return (CalculationHistoryItem::ContentItemStyle) model()->itemStyle();
}

CalculationHistoryItem::ContentItemMode CalculationHistoryItem::itemMode() const
{
	    return (CalculationHistoryItem::ContentItemMode) model()->itemMode();
}


QVariant CalculationHistoryItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value)
{
    if(change == QGraphicsItem::ItemSelectedHasChanged)
    {
        model()->setSelected(value.toBool());
    }
    return QGraphicsItem::itemChange(change, value);
}

