#include "stdafx.h"
#include "Shape.h"
#include <cmath>
#include <complex>
#include <algorithm>

using namespace std;

CShape::CShape()
{
}

CShape::~CShape()
{
	DeleteAllSegments();
}

void CShape::DeleteAllSegments()
{
	for_each(m_vecSegments.begin(), m_vecSegments.end(), DeleteSegment);
	m_vecSegments.clear();
}

CShape::CShape(CShape& original)
{
	operator=(original);
}

CShape& CShape::operator=(CShape& original)
{
	DeleteAllSegments();
	vector<CBezier *>::iterator itr;
	for(itr = original.m_vecSegments.begin(); itr != original.m_vecSegments.end(); itr++)
		m_vecSegments.push_back(new CBezier(**itr));

	return *this;
}

void CShape::Move(const DPoint& delta)
{
	vector<CBezier *>::iterator itr;
	for(itr = m_vecSegments.begin(); itr != m_vecSegments.end(); itr++)
		(*itr)->Move(delta);
}

void CShape::Stretch(const DPoint& base, const DPoint& ratio)
{
	if(m_vecSegments.size() == 0) return;

	vector<CBezier *>::iterator itr;
	for(itr = m_vecSegments.begin(); itr != m_vecSegments.end(); itr++)
		(*itr)->Stretch(base, ratio);
}

void CShape::Render(HDC hDC)
{
	vector<CBezier *>::iterator itr;
	for(itr = m_vecSegments.begin(); itr != m_vecSegments.end(); itr++)
		(*itr)->Render(hDC);
}

// ï¬Ç∂ÇΩã»ê¸Ç»ÇÁÇ±ÇÃï˚ñ@Ç≈îªíËÇ™â¬î\
bool CShape::IsHit(const DPoint& point)
{
	if(GetNSegments() < 1) return false;

	vector<double> vecIntersectionsT;
	vector<double> vecIntersectionsY;
	
	vector<CBezier *>::iterator itr;
	for(itr = m_vecSegments.begin(); itr != m_vecSegments.end(); itr++){
		double max_x, min_x;
		(*itr)->GetXMaxMin(max_x, min_x);
		if(min_x <= point.x && point.x <= max_x){
			(*itr)->GetIntersectionsWithX(point.x, vecIntersectionsT);
			for(int i = 0; i < vecIntersectionsT.size(); i++)
				vecIntersectionsY.push_back((*itr)->GetY(vecIntersectionsT[i]));
			vecIntersectionsT.clear();
		}
	}

	sort(vecIntersectionsY.begin(), vecIntersectionsY.end());
	vector<double>::iterator j;
	for(j = vecIntersectionsY.begin(); j != vecIntersectionsY.end(); j++){
		double y0 = *j;
		if(++j == vecIntersectionsY.end()) break;
		double y1 = *j;
		if(y0 <= point.y && point.y <= y1) return true;
	}

	return false;
}
