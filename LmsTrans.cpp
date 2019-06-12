#include "stdafx.h"
#include "LmsTrans.h"

#undef max
#undef min
extern "C"{
#include "f2c.h"
#include "clapack.h"
}

using namespace std;

CLmsTrans::CLmsTrans()
{
	m_nSamplesPerSegment = 10;
	m_nSamples = 0;
	m_nSegments = 0;
	m_pPoints = NULL;
	m_pMatrix = NULL;
	m_nRows = m_nColumns = 0;
}

CLmsTrans::~CLmsTrans()
{
	delete[] m_pPoints;
	delete[] m_pMatrix;
}

void CLmsTrans::AllocateMemory(int nSegments)
{
	if(m_nSegments == nSegments) return;

	if(m_pPoints){
		delete[] m_pPoints;
		m_pPoints = NULL;
		m_nSamples = 0;
	}

	m_nSegments = nSegments;
	m_nSamples = 1 + (m_nSamplesPerSegment - 1) * nSegments;
	if(m_nSamples > 0) m_pPoints = new DPoint[m_nSamples];

	if(m_pMatrix){
		delete[] m_pMatrix;
		m_pMatrix = NULL;
		m_nRows = m_nColumns = 0;
	}

	m_nRows = 1 + 3 * nSegments;
	m_nColumns = 1 + (m_nSamplesPerSegment - 1) * nSegments;
	if(m_nRows * m_nColumns > 0) m_pMatrix = new double[m_nRows * m_nColumns];

	// Initialize matrix
	PrepareLmsCoeff();
}

void CLmsTrans::PrepareLmsCoeff()
{
	int nRows = m_nColumns;
	int nColumns = m_nRows;

	memset(m_pMatrix, '\0', sizeof(double) * nRows * nColumns);

	m_pMatrix[0] = 1.0;
	for(int i = 1; i < m_nSamplesPerSegment; i++){
		double t = double(i) / (m_nSamplesPerSegment - 1);
		double t1 = 1.0 - t;
		double t2 = t * t;
		double t3 = t1 * t1;
		double c0 = t3 * t1;
		double c1 = 3.0 * t3 * t;
		double c2 = 3.0 * t1 * t2;
		double c3 = t2 * t;
		for(int j = i, k = 0; j < nRows; j += (m_nSamplesPerSegment - 1), k += 3){
			m_pMatrix[j + nRows * k] = c0;
			m_pMatrix[j + nRows * (k + 1)] = c1;
			m_pMatrix[j + nRows * (k + 2)] = c2;
			m_pMatrix[j + nRows * (k + 3)] = c3;
		}
	}

	PseudoInverse(m_pMatrix, nRows, nColumns);
}

// we assume row >= column
bool CLmsTrans::PseudoInverse(double *pMatrix, long row, long column)
{
	double pTemp[13 * 13*10];
	for(int r = 0; r < row; r++)
		for(int s = 0; s < column; s++)
			pTemp[r+row*s]=pMatrix[r+row*s];

	long nrhs, lda, ldb, ldwork, info;
	nrhs = row;
	lda = row;
	ldb = (row > column) ? row : column;
	ldwork = -1;
	double *pB = new double[row * row];
	double optimal_ldwork;

	// get optimal size of pWork
	dgels_("N", &row, &column, &nrhs, pMatrix, &lda, pB, &ldb, &optimal_ldwork, &ldwork, &info);

	// load identity to pB
	memset(pB, '\0', sizeof(double) * row * row);
	for(int i = 0; i < row; i++) pB[i + row * i] = 1.0;

	// calculate least square
	ldwork = (long)optimal_ldwork;
	double *pWork = new double[ldwork];
	dgels_("N", &row, &column, &nrhs, pMatrix, &lda, pB, &ldb, pWork, &ldwork, &info);

	if(info == 0){
		for(int i = 0; i < row; i++){
			for(int j = 0; j < column; j++)
				pMatrix[j + column * i] = pB[j + row * i];
		}
		delete[] pB;
		delete[] pWork;
	}

	return info == 0;
}

void CLmsTrans::PrepareSamples(CShape *pShape)
{
	AllocateMemory(pShape->GetNSegments());

	int i = 0;
	m_pPoints[i++] = (*pShape->m_vecSegments.begin())->begin;
	vector<CBezier *>::iterator itr;
	for(itr = pShape->m_vecSegments.begin(); itr != pShape->m_vecSegments.end(); itr++){
		double x3, x2, x1, x0;
		(*itr)->GetPolynomialCoeffX(x3, x2, x1, x0);
		double y3, y2, y1, y0;
		(*itr)->GetPolynomialCoeffY(y3, y2, y1, y0);
		for(int j = 1; j < m_nSamplesPerSegment; j++, i++){
			double t = double(j) / (m_nSamplesPerSegment - 1);
			m_pPoints[i].x = x3 * t * t * t + x2 * t * t + x1 * t + x0;
			m_pPoints[i].y = y3 * t * t * t + y2 * t * t + y1 * t + y0;
		}
	}
}

void CLmsTrans::ApplyLmsTransform(CShape *pShape)
{
	if(m_nSegments != pShape->GetNSegments()) return;

	int i = 0, j, t;
	double sum;
	vector<CBezier *>::iterator itr = pShape->m_vecSegments.begin();
	for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].x;
	(*itr)->begin.x = sum;
	for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].y;
	(*itr)->begin.y = sum;
	DPoint prev_end;
	for(; itr != pShape->m_vecSegments.end(); itr++){
		if(i != 0) (*itr)->begin = prev_end;
		i++;
		for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].x;
		(*itr)->c1.x = sum;
		for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].y;
		(*itr)->c1.y = sum;
		i++;
		for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].x;
		(*itr)->c2.x = sum;
		for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].y;
		(*itr)->c2.y = sum;
		i++;
		for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].x;
		(*itr)->end.x = sum;
		for(sum = 0.0, t = 0; t < m_nSamples; t++) sum += m_pMatrix[i + m_nRows * t] * m_pPoints[t].y;
		(*itr)->end.y = sum;

		prev_end = (*itr)->end;
	}
}

#if 0
	DPoint *m_pPoints2;
	m_pPoints2 = new DPoint[m_nSamples];


	int nColumn = m_nRows;
	int nRow = m_nColumns;
	for(i = 0; i < nRow; i++){
		double sum_x = 0.0, sum_y = 0.0;
		vector<CBezier *>::iterator itr = pShape->m_vecSegments.begin();
		sum_x = m_pMatrix[i + nRow * 0] * (*itr)->begin.x;
		sum_y = m_pMatrix[i + nRow * 0] * (*itr)->begin.y;
		for(int j = 1; itr != pShape->m_vecSegments.end(); itr++, j++){
			sum_x += m_pMatrix[i + nRow * j] * (*itr)->c1.x;
			sum_y += m_pMatrix[i + nRow * j] * (*itr)->c1.y;
			j++;
			sum_x += m_pMatrix[i + nRow * j] * (*itr)->c2.x;
			sum_y += m_pMatrix[i + nRow * j] * (*itr)->c2.y;
			j++;
			sum_x += m_pMatrix[i + nRow * j] * (*itr)->end.x;
			sum_y += m_pMatrix[i + nRow * j] * (*itr)->end.y;
		}
		m_pPoints2[i].x = sum_x;
		m_pPoints2[i].y = sum_y;
	}

	for(i = 0; i < nRow; i++)
		if(m_pPoints2[i].x != m_pPoints[i].x)
			MessageBox(NULL, "a", "q", MB_OK);

		
		double sum;

		for(int a = 0; a < column; a++){
			for(int b = 0; b < column; b++){
				sum = 0;
				for(int c = 0; c < row; c++){
					sum +=pTemp[c + row * a] * pB[b + row * c];
				}
				char szBuff[200];
				sprintf(szBuff,"%2.2f  ",sum);
				strcat(szBufff, szBuff);
			}
				MessageBox(NULL, szBufff, "hello", MB_OK);
				szBufff[0]='\0';
		}
	char szBufff[500];
	szBufff[0]='\0';

#endif
