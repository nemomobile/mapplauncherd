#ifndef SAVEDCALCULATIONITEMSTYLE_H
#define SAVEDCALCULATIONITEMSTYLE_H

#include <MWidgetStyle>

class SavedCalculationItemStyle : public MWidgetStyle
{
	Q_OBJECT
	M_STYLE(SavedCalculationItemStyle)

	M_STYLE_PTR_ATTRIBUTE(MScalableImage*, backgroundImageSingle, BackgroundImageSingle)
	M_STYLE_PTR_ATTRIBUTE(MScalableImage*,  backgroundImageSinglecolumnTop,    BackgroundImageSinglecolumnTop)
	M_STYLE_PTR_ATTRIBUTE(MScalableImage*,  backgroundImageSinglecolumnCenter,    BackgroundImageSinglecolumnCenter)
	M_STYLE_PTR_ATTRIBUTE(MScalableImage*,  backgroundImageSinglecolumnBottom,    BackgroundImageSinglecolumnBottom)
};

class SavedCalculationItemStyleContainer : public MWidgetStyleContainer
{
	M_STYLE_CONTAINER(SavedCalculationItemStyle)
};

#endif

