#include "savedcalculationitem.h"
#include "savedcalculationitemview.h"

SavedCalculationItem::SavedCalculationItem(MWidget *parent)
	  //:MWidgetController(parent, new SavedCalculationItemModel)
	  :MWidgetController(new SavedCalculationItemModel, parent)
{}

SavedCalculationItem::~SavedCalculationItem()
{
}


void SavedCalculationItem::emitClickedSignal()
{
	emit clicked();
}

void SavedCalculationItem::emitSavedCalculationItemStateChanged()
{
	emit savedcalculationitemStateChanged();
}

void SavedCalculationItem::setItemMode(ContentItemMode mode)
{
    model()->setItemMode(mode);
}

void SavedCalculationItem::setTimeOfCalculation(const QString& timeOfCalc )
{
	model()->setTimeOfCalculation( timeOfCalc);
}

void SavedCalculationItem::setResult(const QString& result )
{
	model()->setResult( result );
}

void SavedCalculationItem::setInput(const QString& input )
{
	model()->setInput( input );
}

void SavedCalculationItem::setIndex( const QString& index )
{
    model()->setIndex( index );
}

void SavedCalculationItem::setName( const QString& name )
{
	model()->setName( name );
}

const QString& SavedCalculationItem::timeOfCalculation()
{
	return model()->timeOfCalculation();
}

const QString& SavedCalculationItem::result()
{
	return model()->result();
}

const QString& SavedCalculationItem::input()
{
	return model()->input();
}

const QString& SavedCalculationItem::index()
{
    return model()->index();
}


const QString& SavedCalculationItem::name()
{
	return model()->name();
}

SavedCalculationItem::ContentItemStyle SavedCalculationItem::itemStyle() const
{
    return (SavedCalculationItem::ContentItemStyle) model()->itemStyle();
}

SavedCalculationItem::ContentItemMode SavedCalculationItem::itemMode() const
{
    return (SavedCalculationItem::ContentItemMode) model()->itemMode();
}

