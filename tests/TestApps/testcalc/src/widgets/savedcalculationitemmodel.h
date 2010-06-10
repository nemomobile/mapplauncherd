#ifndef SAVEDCALCULATIONITEMMODEL_H
#define SAVEDCALCULATIONITEMMODEL_H

#include <MWidgetModel>

class SavedCalculationItemModel : public MWidgetModel
{
	Q_OBJECT
	M_MODEL(SavedCalculationItemModel)

	M_MODEL_PROPERTY(QString, timeOfCalculation, TimeOfCalculation, true, "")
	M_MODEL_PROPERTY(QString, result, Result, true, "")
	M_MODEL_PROPERTY(QString, input, Input, true, "")
	M_MODEL_PROPERTY(QString, index, Index, true, "")
	M_MODEL_PROPERTY(QString, name, Name, true, "")
	M_MODEL_PROPERTY(int, itemStyle, ItemStyle, true, 0)
	M_MODEL_PROPERTY(int, itemMode, ItemMode, true, 0)
};

#endif
