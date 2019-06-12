#ifndef DEF_FIGURE
#define DEF_FIGURE

#include "Point.h"
#include "Shape.h"

class CFigure
{
public:
	static CShape *CreateLine(const DPoint& p1, const DPoint& p2);
	static CShape *CreateRectangle(double left, double top, double right, double bottom);
	static CShape *CreatePolygon(DPoint *pPoints, int nPoints);
	static CShape *CreateCircle(const DPoint& center, double radius);
	static CShape *CreateEllipse(double left, double top, double right, double bottom);
	static CShape *CreateTriangle(double left, double top, double right, double bottom);
	static CShape *CreateHeart(double left, double top, double right, double bottom);
	static void ApplyHandTransform(CShape *pShape, const DPoint& delta, const DPoint& pos, double radius);
};

#endif
