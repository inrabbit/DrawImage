#ifndef DEF_CBEZIER
#define DEF_CBEZIER

#include <vector>
#include "Point.h"

class CBezier
{
public:
	DPoint begin, c1, c2, end;
public:
	CBezier(){}
	CBezier(CBezier& original);
public:
	void GetXMaxMin(double& Xmax, double& Xmin);
	void GetIntersectionsWithX(double x, std::vector<double>& vecIntersections);
	void GetIntersectionsWithLine(DPoint& pos, DPoint& vec, std::vector<double>& vecIntersections);
	void GetIntersectionsWithBezier(CBezier& bezier, std::vector<double>& vecIntersections);
public:
	double GetX(double t);
	double GetY(double t);
	void Render(HDC hDC);
	void Move(const DPoint& delta);
	void Stretch(const DPoint& base, const DPoint& ratio);
public:
	void GetPolynomialCoeffX(double& x3, double& x2, double& x1, double& x0);
	void GetPolynomialCoeffY(double& y3, double& y2, double& y1, double& y0);
	void SolvePolynomial(double x3, double x2, double x1, double x0, std::vector<double>& vecSolutions);
	void SolvePolynomial(double x2, double x1, double x0, std::vector<double>& vecSolutions);
	void SolvePolynomial(double x1, double x0, std::vector<double>& vecSolutions);
};

#endif
