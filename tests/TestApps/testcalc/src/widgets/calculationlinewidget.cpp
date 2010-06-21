#include "calculationlinewidget.h"
#include "calculationlinewidgetview.h"

CalculationLineWidget::CalculationLineWidget(MWidget *parent)
	  :MWidgetController(new CalculationLineWidgetModel, parent)
{
	setFocusPolicy(Qt::StrongFocus);
	setFlag(QGraphicsItem::ItemIsFocusable);
}

CalculationLineWidget::~CalculationLineWidget()
{
}

void CalculationLineWidget::emitClickedSignal()
{
	emit clicked();
}

void CalculationLineWidget::emitCalculationLineWidgetStateChanged()
{
	emit calculationlinewidgetStateChanged();
}

void CalculationLineWidget::setText(const QString& text )
{
	model()->setText( text );
}

const QString& CalculationLineWidget::text()
{
	return model()->text();
}

void CalculationLineWidget::setItemMode(ContentItemMode mode)
{
model()->setItemMode(mode);
}

CalculationLineWidget::ContentItemMode CalculationLineWidget::itemMode() const
{
    return (CalculationLineWidget::ContentItemMode) model()->itemMode();
}



QVariant CalculationLineWidget::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value)
{
    if(change == QGraphicsItem::ItemSelectedHasChanged)
    {
        model()->setSelected(value.toBool());
    }
    return QGraphicsItem::itemChange(change, value);
}

