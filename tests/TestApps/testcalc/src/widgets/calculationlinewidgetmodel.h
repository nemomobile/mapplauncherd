#ifndef CALCULATIONLINEWIDGETMODEL_H
#define CALCULATIONLINEWIDGETMODEL_H

#include <MWidgetModel>

class CalculationLineWidgetModel : public MWidgetModel
{
	Q_OBJECT
	M_MODEL(CalculationLineWidgetModel)

	M_MODEL_PROPERTY(QString, text, Text, true, "")
	M_MODEL_PROPERTY(bool, selected, Selected, true, false)
	M_MODEL_PROPERTY(int, itemMode, ItemMode, true, 0)
};

#endif
