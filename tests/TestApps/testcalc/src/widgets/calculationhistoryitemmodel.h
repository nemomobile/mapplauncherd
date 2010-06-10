#ifndef CALCULATIONHISTORYITEMMODEL_H
#define CALCULATIONHISTORYITEMMODEL_H

#include <MWidgetModel>

class CalculationHistoryItemModel : public MWidgetModel
{
	Q_OBJECT
	M_MODEL(CalculationHistoryItemModel)

	M_MODEL_PROPERTY(QString, timeOfCalculation, TimeOfCalculation, true, "")
	M_MODEL_PROPERTY(QString, result, Result, true, "")
	M_MODEL_PROPERTY(QString, input, Input, true, "")
	M_MODEL_PROPERTY(QString, index, Index, true, "" )
	M_MODEL_PROPERTY(int, itemStyle, ItemStyle, true, 0)
	M_MODEL_PROPERTY(int, itemMode, ItemMode, true, 0)
	M_MODEL_PROPERTY(bool, selected, Selected, true, false)
	M_MODEL_PROPERTY(Qt::TextElideMode, elide, Elide, true, Qt::ElideNone)
};

#endif
