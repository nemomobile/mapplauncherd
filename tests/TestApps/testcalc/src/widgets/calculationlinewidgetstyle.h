#ifndef CALCULATIONLINEWIDGETITEMSTYLE_H
#define CALCULATIONLINEWIDGETITEMSTYLE_H

#include <MWidgetStyle>

class CalculationLineWidgetStyle : public MWidgetStyle
{
	Q_OBJECT
	M_STYLE(CalculationLineWidgetStyle)

	M_STYLE_PTR_ATTRIBUTE(MScalableImage*, backgroundImageSingle, BackgroundImageSingle)
};

class CalculationLineWidgetStyleContainer : public MWidgetStyleContainer
{
	M_STYLE_CONTAINER(CalculationLineWidgetStyle)
};

#endif

