#ifndef DEF_CSHAPE
#define DEF_CSHAPE

#include <vector>
#include "Bezier.h"

class CShape
{
public:
	std::vector<CBezier *> m_vecSegments;
public:
	CShape();
	CShape(CShape& original);
	virtual ~CShape();
	CShape& operator=(CShape& original);
public:
	void AddSegment(CBezier *pSegment){ m_vecSegments.push_back(pSegment); }
	void Render(HDC hDC);
	bool IsHit(const DPoint& point);
	void Move(const DPoint& delta);
	void Stretch(const DPoint& base, const DPoint& ratio);

	int GetNSegments(){ return m_vecSegments.size(); }
protected:
	void DeleteAllSegments();
	static void DeleteSegment(void *p){ delete p; }
};

#endif
