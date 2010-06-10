#ifndef CALCULATIONHISTORYITEMSTYLE_H
#define CALCULATIONHISTORYITEMSTYLE_H

#include <MWidgetStyle>

class CalculationHistoryItemStyle : public MWidgetStyle
{
	Q_OBJECT
	M_STYLE(CalculationHistoryItemStyle)

	M_STYLE_PTR_ATTRIBUTE(MScalableImage*, backgroundImageSingle, BackgroundImageSingle)
	M_STYLE_PTR_ATTRIBUTE(MScalableImage*,  backgroundImageSinglecolumnTop,    BackgroundImageSinglecolumnTop)
	M_STYLE_PTR_ATTRIBUTE(MScalableImage*,  backgroundImageSinglecolumnCenter,    BackgroundImageSinglecolumnCenter)
	M_STYLE_PTR_ATTRIBUTE(MScalableImage*,  backgroundImageSinglecolumnBottom,    BackgroundImageSinglecolumnBottom)
};

class CalculationHistoryItemStyleContainer : public MWidgetStyleContainer
{
	M_STYLE_CONTAINER(CalculationHistoryItemStyle)
};

#endif

