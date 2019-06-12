#include "stdafx.h"
#include "Bezier.h"
#include <cmath>
#include <complex>
#include <algorithm>

using namespace std;

#define EPTHIRON 0.00001

CBezier::CBezier(CBezier& original)
{
	*this = original;
}

double CBezier::GetX(double t)
{
	double t1 = 1.0 - t;
	double t2 = t * t;
	double t3 = t1 * t1;
	return begin.x * t3 * t1 + 3.0 * (c1.x * t3 * t + c2.x * t1 * t2) + end.x * t2 * t;
}

double CBezier::GetY(double t)
{
	double t1 = 1.0 - t;
	double t2 = t * t;
	double t3 = t1 * t1;
	return begin.y * t3 * t1 + 3.0 * (c1.y * t3 * t + c2.y * t1 * t2) + end.y * t2 * t;
}

void CBezier::Render(HDC hDC)
{
	POINT szPoints[3];

	szPoints[0].x = c1.x;
	szPoints[0].y = c1.y;
	szPoints[1].x = c2.x;
	szPoints[1].y = c2.y;
	szPoints[2].x = end.x;
	szPoints[2].y = end.y;

	MoveToEx(hDC, begin.x, begin.y, NULL);
	PolyBezierTo(hDC , szPoints, 3);
}

void CBezier::Move(const DPoint& delta)
{
	begin += delta;
	c1 += delta;
	c2 += delta;
	end += delta;
}

void CBezier::Stretch(const DPoint& base, const DPoint& ratio)
{
	DPoint delta = begin - base;
	begin = DPoint(ratio.x * delta.x, ratio.y * delta.y) + base;
	delta = c1 - base;
	c1 = DPoint(ratio.x * delta.x, ratio.y * delta.y) + base;
	delta = c2 - base;
	c2 = DPoint(ratio.x * delta.x, ratio.y * delta.y) + base;
	delta = end - base;
	end = DPoint(ratio.x * delta.x, ratio.y * delta.y) + base;
}

void CBezier::GetXMaxMin(double& Xmax, double& Xmin)
{
	vector<double> vecCandidates;
	vecCandidates.push_back(0.0);
	vecCandidates.push_back(1.0);

	double x3, x2, x1, x0;
	GetPolynomialCoeffX(x3, x2, x1, x0);

	// ｔで微分した式を解く
	SolvePolynomial(x3 * 3.0, x2 * 2.0, x1, vecCandidates);

	vector<double>::iterator itr;
	for(itr = vecCandidates.begin(); itr != vecCandidates.end(); itr++)
		*itr = GetX(*itr);

	Xmax = *max_element(vecCandidates.begin(), vecCandidates.end());
	Xmin = *min_element(vecCandidates.begin(), vecCandidates.end());
}

void CBezier::GetIntersectionsWithX(double x, std::vector<double>& vecIntersections)
{
	double x3, x2, x1, x0;
	GetPolynomialCoeffX(x3, x2, x1, x0);

	SolvePolynomial(x3, x2, x1, x0 - x, vecIntersections);
}

// posを通るvec向きの直線との交点を求める
void CBezier::GetIntersectionsWithLine(DPoint& pos, DPoint& vec, std::vector<double>& vecIntersections)
{
	double x3, x2, x1, x0;
	GetPolynomialCoeffX(x3, x2, x1, x0);

	if(vec.x == 0.0){
		SolvePolynomial(x3, x2, x1, x0 - pos.x, vecIntersections);
	}else{
		double a = vec.y / vec.x;
		double b = - pos.x * a + pos.y;
		SolvePolynomial(x3, x2, x1 - a, x0 - b, vecIntersections);
	}
}

void CBezier::GetIntersectionsWithBezier(CBezier& bezier, std::vector<double>& vecIntersections)
{
	double x3, x2, x1, x0;
	GetPolynomialCoeffX(x3, x2, x1, x0);
	double y3, y2, y1, y0;
	bezier.GetPolynomialCoeffX(y3, y2, y1, y0);

	SolvePolynomial(x3 - y3, x2 - y2, x1 - y1, x0 - y0, vecIntersections);
}

// ベジェ曲線の方程式の係数を得る（X座標）
void CBezier::GetPolynomialCoeffX(double& x3, double& x2, double& x1, double& x0)
{
	x3 = - begin.x + 3.0 * c1.x - 3.0 * c2.x + end.x;
	x2 = 3.0 * begin.x - 6.0 * c1.x + 3.0 * c2.x;
	x1 = -3.0 * begin.x + 3.0 * c1.x;
	x0 = begin.x;
}

// ベジェ曲線の方程式の係数を得る（Y座標）
void CBezier::GetPolynomialCoeffY(double& y3, double& y2, double& y1, double& y0)
{
	y3 = - begin.y + 3.0 * c1.y - 3.0 * c2.y + end.y;
	y2 = 3.0 * begin.y - 6.0 * c1.y + 3.0 * c2.y;
	y1 = -3.0 * begin.y + 3.0 * c1.y;
	y0 = begin.y;
}

// ３次方程式を解く
void CBezier::SolvePolynomial(double x3, double x2, double x1, double x0, std::vector<double>& vecSolutions)
{
	if(x3 == 0.0){
		SolvePolynomial(x2, x1, x0, vecSolutions);
		return;
	}

	double a = x2 / x3;
	double b = x1 / x3;
	double c = x0 / x3;

	// ３次方程式の解の公式
	double p = b - a * a / 3;
	double q = 2.0 * a * a * a / 27 - a * b / 3 + c;
	complex<double> w(- 1.0 / 2, sqrt(3) / 2);
	complex<double> r(q * q / 4 + p * p * p / 27, 0);
	complex<double> m = pow(- q / 2 + sqrt(r), 1.0 / 3);
	complex<double> n = pow(- q / 2 - sqrt(r), 1.0 / 3);

	complex<double> t[3];
	t[0] = m + n - a / 3;
	t[1] = w * m + w * w * n - a / 3;
	t[2] = w * w * m + w * n - a / 3;

	for(int i = 0; i < 3; i++){
		double tr = t[i].real();
		double ti = t[i].imag();
		if(ti < 0) ti = -ti;
		if(ti < EPTHIRON && 0 <= tr && tr <= 1){
			vecSolutions.push_back(tr);
		}
	}
}

// ２次方程式を解く
void CBezier::SolvePolynomial(double x2, double x1, double x0, std::vector<double>& vecSolutions)
{
	if(x2 == 0.0){
		SolvePolynomial(x1, x0, vecSolutions);
		return;
	}

	double disc = x1 * x1 - 4.0 * x2 * x0;
	if(disc > 0){
		double ans1 = (- x1 + sqrt(disc)) / (2.0 * x2);
		double ans2 = (- x1 - sqrt(disc)) / (2.0 * x2);
		if(0 < ans1 && ans1 < 1) vecSolutions.push_back(ans1);
		if(0 < ans2 && ans2 < 1) vecSolutions.push_back(ans2);
	}else if(disc == 0.0){
		double ans = - x1 / (2.0 * x2);
		if(0 < ans && ans < 1) vecSolutions.push_back(ans);
	}
}

// １次方程式を解く
void CBezier::SolvePolynomial(double x1, double x0, std::vector<double>& vecSolutions)
{
	if(x1 != 0.0){
		double ans = x0 / x1;
		if(0.0 < ans && ans < 1.0) vecSolutions.push_back(ans);
	}
}
