#ifndef DEF_LMS_TRANS
#define DEF_LMS_TRANS

#include "Point.h"
#include "Bezier.h"
#include "Shape.h"

class CLmsTrans
{
private:
	int m_nSamplesPerSegment;
	int m_nSamples;
	int m_nSegments;
	DPoint *m_pPoints;
	double *m_pMatrix;
	int m_nRows, m_nColumns;
public:
	CLmsTrans();
	~CLmsTrans();

	void PrepareSamples(CShape *pShape);
	void ApplyLmsTransform(CShape *pShape);
	DPoint *GetSamples(){ return m_pPoints; }
	int GetNSamples(){ return m_nSamples; }
private:
	void AllocateMemory(int nSegments);
	void PrepareLmsCoeff();
	bool PseudoInverse(double *pMatrix, long row, long column);
};

#endif
