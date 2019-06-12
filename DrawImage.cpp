#include "stdafx.h"
#include "resource.h"
#include <wingdi.h>
#include "main.h"
//
//  �֐�: WndProc(HWND, unsigned, WORD, LONG)
//
//  �p�r: ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- �I�����b�Z�[�W�̒ʒm�ƃ��^�[��
//
//

#include <vector>
#include "Shape.h"
#include "Figure.h"

CShape* FindActiveShape(POINT point);

using namespace std;


// Global variables

#define HAND_RADIUS 60

POINTS downPoint;				// �}�E�X�̍��{�^�������������̍��W
POINTS lastPoint;				// �Ō�Ƀ}�E�X�C�x���g�����������Ƃ��̃}�E�X�̍��W�B
POINTS currPoint;

vector<CShape*> shapes;

CShape* pActiveShape = NULL;		// Active�Ȑ}�`�I�u�W�F�N�g�ւ̃|�C���^
CShape* pTempShape = NULL;

enum SHAPE_KIND		// �쐬����}�`�̎��
{
	SK_ELLIPSE,
	SK_RECTANGLE,
	SK_HEART,
};
SHAPE_KIND	shapeKind = SK_ELLIPSE;	// �쐬����}�`�̎�ނ�ێ�����B
enum { create, move, hand } ToolMode = create;
bool IsMouseDown = false;

HDC		hMemDC = NULL;
HBITMAP	hMemBitmap = NULL;

// Implementation

void MoveMouseMove(HWND hWnd, POINTS point)
{
	// �}�`���ړ�����B
	// pActiveShape��NULL�̎��i�V�K�쐬���[�h�j�͉������Ȃ��B
	if( pActiveShape != NULL ){
		// �O��̃}�E�X�C�x���g����̈ړ��ʂ����߂�
		DPoint delta;
		delta.x = point.x - lastPoint.x; 
		delta.y = point.y - lastPoint.y;
		// �A�N�e�B�u�Ȑ}�`���ړ�����B
		pActiveShape->Move( delta );
	}
}

void CreateMouseMove(HWND hWnd, POINTS point, PAINTSTRUCT& ps)
{
	//	���{�^�������������W�Ɨ��������W��Ίp���Ɏ��}�`��`�悷��B
	int left = (downPoint.x < point.x) ? downPoint.x : point.x;
	int right = (downPoint.x < point.x) ? point.x : downPoint.x;
	int top = (downPoint.y < point.y) ? downPoint.y : point.y;
	int bottom = (downPoint.y < point.y) ? point.y : downPoint.y;

	// ���j���[�őI������Ă���}�`�𐶐�����
	CShape *pShape;
	switch(shapeKind){
	case SK_RECTANGLE:
		pShape = CFigure::CreateRectangle(left, top, right, bottom);
		break;
	case SK_ELLIPSE:
		pShape = CFigure::CreateEllipse(left, top, right, bottom);
		break;
	case SK_HEART:
		pShape = CFigure::CreateHeart(left, top, right, bottom);
		break;
	}

	if(pTempShape) delete pTempShape;
	pTempShape = pShape;
}

void CreateMouseUp(HWND hWnd, POINTS upPoint, SHAPE_KIND shapeKind)
{
	CShape* pShape;
	//	���{�^�������������W�Ɨ��������W��Ίp���Ɏ��}�`��`�悷��B
	int left = (downPoint.x < upPoint.x) ? downPoint.x : upPoint.x;
	int right = (downPoint.x < upPoint.x) ? upPoint.x : downPoint.x;
	int top = (downPoint.y < upPoint.y) ? downPoint.y : upPoint.y;
	int bottom = (downPoint.y < upPoint.y) ? upPoint.y : downPoint.y;

	// ���j���[�őI������Ă���}�`�𐶐�����
	switch(shapeKind){
	case SK_RECTANGLE:
		pShape = CFigure::CreateRectangle(left, top, right, bottom);
		break;
	case SK_ELLIPSE:
		pShape = CFigure::CreateEllipse(left, top, right, bottom);
		break;
	case SK_HEART:
		pShape = CFigure::CreateHeart(left, top, right, bottom);
		break;
	}

	// ���������}�`���R���N�V�����ɒǉ�����B
	shapes.push_back( pShape );
}

void PrintMode(HDC hdc)
{
	static char *pszModeString[] = {
		"�ȉ~�̍쐬",
		"�����`�̍쐬",
		"�}�`�̈ړ�",
		"��������ό`",
		"�n�[�g�̍쐬"};

	// ���݂̃��[�h�\��
	char *pMode = NULL;
	switch(ToolMode){
	case move:
		pMode = pszModeString[2];
		break;
	case hand:
		pMode = pszModeString[3];
		break;
	case create:
		switch(shapeKind){
			case SK_RECTANGLE:
				pMode = pszModeString[1];
				break;
			case SK_ELLIPSE:
				pMode = pszModeString[0];
				break;
			case SK_HEART:
				pMode = pszModeString[4];
				break;
		}
		break;
	}
	TextOut(hdc, 10, 10, pMode, strlen(pMode));
}

void OnPaint(HDC hdc)
{
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));

	// �R���N�V�������̂��ׂĂ̐}�`��`�悷��
	for( int i = 0; i < shapes.size(); i++ )
		shapes[i]->Render(hdc);

	if(pTempShape){
		HPEN hpen = CreatePen(PS_DOT, 0 , (COLORREF)0x000000ff);
		HPEN holdpen = (HPEN)SelectObject(hdc , hpen);
		pTempShape->Render(hdc);
		SelectObject(hdc , holdpen);
		DeleteObject(hpen);
	}

	PrintMode(hdc);

	if(ToolMode == hand){
		HPEN hpen = CreatePen(PS_DOT, 0 , (COLORREF)0x000000ff);
		HPEN holdpen = (HPEN)SelectObject(hdc , hpen);
		HBRUSH hbrush = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
		HBRUSH holdbrh = (HBRUSH)SelectObject(hdc , hbrush);
		Ellipse(hdc, currPoint.x - HAND_RADIUS, currPoint.y - HAND_RADIUS, currPoint.x + HAND_RADIUS, currPoint.y + HAND_RADIUS);
		SelectObject(hdc , holdpen);
		SelectObject(hdc, holdbrh);
		DeleteObject(hpen);
	}

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message ) 
	{
		case WM_CREATE:
			{
				hMemDC		 = CreateCompatibleDC(NULL);
				hMemBitmap   = CreateCompatibleBitmap(hMemDC, 1024, 768);
				SelectObject( hMemDC, hMemBitmap );
				break;
			}
		case WM_LBUTTONDOWN:
			{
				SetCapture(hWnd);
				//�@���{�^�����������Ƃ��̍��W��ێ�����B
				downPoint = lastPoint = MAKEPOINTS(lParam);	

				// �}�E�X�����̐}�`����������B
				// ���������ꍇ�́ApActiveShape�ɑ�����A�h���b�O�̑ΏۂƂ���B
				// ��������Ȃ������ꍇ�́ApActiveShape = NULL
				//   -> �V�K�̐}�`��ǉ����郂�[�h
				POINT current;
				current.x = downPoint.x;
				current.y = downPoint.y;
				IsMouseDown = true;

				pActiveShape = FindActiveShape( current );

				break;
			}
		case WM_MOUSEMOVE:
			{
				//if( GetCapture() != hWnd ) break;
				POINTS point = MAKEPOINTS(lParam);	

				int i;
				DPoint delta, pos;

				switch(ToolMode){
				case move:
					MoveMouseMove(hWnd, point);
					if(pActiveShape) InvalidateRect(hWnd, NULL, FALSE);
					break;
				case create:
					if(IsMouseDown){
						CreateMouseMove(hWnd, point, ps);
						InvalidateRect(hWnd, NULL, FALSE);
					}
					break;
				case hand:
					currPoint = point;
					InvalidateRect(hWnd, NULL, FALSE);
					if(IsMouseDown){
						delta.x = point.x - lastPoint.x;
						delta.y = point.y - lastPoint.y;
						pos.x = lastPoint.x;
						pos.y = lastPoint.y;

						for(i = 0; i < shapes.size(); i++)
							CFigure::ApplyHandTransform(shapes[i], delta, pos, HAND_RADIUS);
					}
					break;
				default:
					MessageBox(hWnd, "���[�h���I������Ă��܂���", "��������", MB_OK);
				}

				lastPoint = MAKEPOINTS(lParam);
				break;
			}
		case WM_LBUTTONUP:
			{
				if( GetCapture() != hWnd ) break;
				ReleaseCapture();
				POINTS upPoint = MAKEPOINTS(lParam);

				switch(ToolMode){
				case move:
					break;
				case hand:
					break;
				case create:
					if(pTempShape) delete pTempShape;
					pTempShape = NULL;
					CreateMouseUp(hWnd, upPoint, shapeKind);
					InvalidateRect(hWnd, NULL, FALSE);
					break;
				}
				pActiveShape = NULL;
				IsMouseDown = false;
			}
			break;
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// �ĕ`��
			InvalidateRect(hWnd, NULL, FALSE);
			// ���j���[�I���̉��:
			switch( wmId ) 
			{
				case ID_SHAPE_ELLIPSE:
					ToolMode = create;
					shapeKind = SK_ELLIPSE;
					break;
				case ID_SHAPE_RECTANGLE:
					ToolMode = create;
					shapeKind = SK_RECTANGLE;
					break;
				case ID_SHAPE_HEART:
					ToolMode = create;
					shapeKind = SK_HEART;
					break;
				case ID_TOOL_HAND:
					ToolMode = hand;
					break;
				case ID_TOOL_MOVE:
					ToolMode = move;
					break;
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow( hWnd );
				   break;
				default:
				   return DefWindowProc( hWnd, message, wParam, lParam );
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint (hWnd, &ps);

				// ������DC���N���A
				RECT rect;
				GetClientRect(hWnd, &rect);
				FillRect( hMemDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

				OnPaint(hMemDC);

				// ������DC����ʂɓ]��
				BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

				EndPaint( hWnd, &ps );
				break;
			}
		case WM_DESTROY:
			DeleteObject( hMemBitmap );
			DeleteDC( hMemDC );
			PostQuitMessage( 0 );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
   }
   return 0;
}

// �}�E�X�����̐}�`����������B
CShape* FindActiveShape(POINT point)
{
	for(int i = shapes.size() - 1; i >= 0; i--)
		if(shapes[i]->IsHit(DPoint(point.x, point.y))) return shapes[i];

	return NULL;
}

// �o�[�W�������{�b�N�X�p���b�Z�[�W �n���h��
LRESULT CALLBACK About( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if( LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL ) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

