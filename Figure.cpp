#include "stdafx.h"
#include "Figure.h"
#include "LmsTrans.h"
#include <cmath>

CShape *CFigure::CreateLine(const DPoint& p1, const DPoint& p2)
{
	// Not implemented yet
	return NULL;
}

CShape *CFigure::CreateRectangle(double left, double top, double right, double bottom)
{
	CShape *pShape = new CShape();
	CBezier *pSegment;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = left;
	pSegment->begin.y = top;
	pSegment->c1 = pSegment->begin;
	pSegment->c2.x = left;	// ”½ŽžŒv‰ñ‚è
	pSegment->c2.y = bottom;
	pSegment->end = pSegment->c2;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = left;
	pSegment->begin.y = bottom;
	pSegment->c1 = pSegment->begin;
	pSegment->c2.x = right;
	pSegment->c2.y = bottom;
	pSegment->end = pSegment->c2;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = right;
	pSegment->begin.y = bottom;
	pSegment->c1 = pSegment->begin;
	pSegment->c2.x = right;
	pSegment->c2.y = top;
	pSegment->end = pSegment->c2;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = right;
	pSegment->begin.y = top;
	pSegment->c1 = pSegment->begin;
	pSegment->c2.x = left;
	pSegment->c2.y = top;
	pSegment->end = pSegment->c2;

	return pShape;
}

CShape *CFigure::CreatePolygon(DPoint *pPoints, int nPoints)
{
	// Not implemented yet
	return NULL;
}

#define CIRCLE_HANDLE_RATIO 0.552283	// handle length to radius
CShape *CFigure::CreateCircle(const DPoint& center, double radius)
{
	double tangent = CIRCLE_HANDLE_RATIO * radius;

	CShape *pShape = new CShape();
	CBezier *pSegment;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = center.x + radius;
	pSegment->begin.y = center.y;
	pSegment->c1.x = center.x + radius;	// ”½ŽžŒv‰ñ‚è
	pSegment->c1.y = center.y - tangent;
	pSegment->c2.x = center.x + tangent;
	pSegment->c2.y = center.y - radius;
	pSegment->end.x = center.x;
	pSegment->end.y = center.y - radius;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = center.x;
	pSegment->begin.y = center.y - radius;
	pSegment->c1.x = center.x - tangent;
	pSegment->c1.y = center.y - radius;
	pSegment->c2.x = center.x - radius;
	pSegment->c2.y = center.y - tangent;
	pSegment->end.x = center.x - radius;
	pSegment->end.y = center.y;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = center.x - radius;
	pSegment->begin.y = center.y;
	pSegment->c1.x = center.x - radius;
	pSegment->c1.y = center.y + tangent;
	pSegment->c2.x = center.x - tangent;
	pSegment->c2.y = center.y + radius;
	pSegment->end.x = center.x;
	pSegment->end.y = center.y + radius;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = center.x;
	pSegment->begin.y = center.y + radius;
	pSegment->c1.x = center.x + tangent;
	pSegment->c1.y = center.y + radius;
	pSegment->c2.x = center.x + radius;
	pSegment->c2.y = center.y + tangent;
	pSegment->end.x = center.x + radius;
	pSegment->end.y = center.y;

	return pShape;
}

CShape *CFigure::CreateEllipse(double left, double top, double right, double bottom)
{
	double width = right - left;
	double height = bottom - top;
	double radius = height / 2;

	DPoint center;
	center.x = left + radius;
	center.y = top + radius;

	double ratio_x = double(width) / height;

	CShape *pShape = CreateCircle(center, radius);
	pShape->Stretch(DPoint(left, top), DPoint(ratio_x, 1.0));

	return pShape;
}

CShape *CFigure::CreateTriangle(double left, double top, double right, double bottom)
{
	// Not implemented yet
	return NULL;
}

CShape *CFigure::CreateHeart(double left, double top, double right, double bottom)
{
	double width = right - left;
	double height = bottom - top;

	DPoint center;
	center.x = left + width / 2;
	center.y = top + height / 2;

	CShape *pShape = new CShape();
	CBezier *pSegment;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = center.x;
	pSegment->begin.y = bottom;
	pSegment->c1.x = center.x;	// ”½ŽžŒv‰ñ‚è
	pSegment->c1.y = bottom;
	pSegment->c2.x = center.x + width / 2;
	pSegment->c2.y = top - width / 2;
	pSegment->end.x = center.x;
	pSegment->end.y = top;

	pShape->AddSegment(pSegment = new CBezier());
	pSegment->begin.x = center.x;
	pSegment->begin.y = top;
	pSegment->c1.x = center.x - width / 2;
	pSegment->c1.y = top - width / 2;
	pSegment->c2.x = center.x;
	pSegment->c2.y = bottom;
	pSegment->end.x = center.x;
	pSegment->end.y = bottom;

	return pShape;
}

void CFigure::ApplyHandTransform(CShape *pShape, const DPoint& delta, const DPoint& pos, double radius)
{
	static CLmsTrans LmsTrans;
	LmsTrans.PrepareSamples(pShape);
	DPoint *pSamples = LmsTrans.GetSamples();

	for(int i = 0; i < LmsTrans.GetNSamples(); i++){
		DPoint dist = pSamples[i] - pos;
		double norm = dist.norm();
		pSamples[i] += delta * exp(-norm / 800);
	}

	LmsTrans.ApplyLmsTransform(pShape);
}
