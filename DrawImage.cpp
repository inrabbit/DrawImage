#include "stdafx.h"
#include "resource.h"
#include <wingdi.h>
#include "main.h"
//
//  関数: WndProc(HWND, unsigned, WORD, LONG)
//
//  用途: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 終了メッセージの通知とリターン
//
//

#include <vector>
#include "Shape.h"
#include "Figure.h"

CShape* FindActiveShape(POINT point);

using namespace std;


// Global variables

#define HAND_RADIUS 60

POINTS downPoint;				// マウスの左ボタンを押した時の座標
POINTS lastPoint;				// 最後にマウスイベントが発生したときのマウスの座標。
POINTS currPoint;

vector<CShape*> shapes;

CShape* pActiveShape = NULL;		// Activeな図形オブジェクトへのポインタ
CShape* pTempShape = NULL;

enum SHAPE_KIND		// 作成する図形の種類
{
	SK_ELLIPSE,
	SK_RECTANGLE,
	SK_HEART,
};
SHAPE_KIND	shapeKind = SK_ELLIPSE;	// 作成する図形の種類を保持する。
enum { create, move, hand } ToolMode = create;
bool IsMouseDown = false;

HDC		hMemDC = NULL;
HBITMAP	hMemBitmap = NULL;

// Implementation

void MoveMouseMove(HWND hWnd, POINTS point)
{
	// 図形を移動する。
	// pActiveShapeがNULLの時（新規作成モード）は何もしない。
	if( pActiveShape != NULL ){
		// 前回のマウスイベントからの移動量を求める
		DPoint delta;
		delta.x = point.x - lastPoint.x; 
		delta.y = point.y - lastPoint.y;
		// アクティブな図形を移動する。
		pActiveShape->Move( delta );
	}
}

void CreateMouseMove(HWND hWnd, POINTS point, PAINTSTRUCT& ps)
{
	//	左ボタンを押した座標と離した座標を対角線に持つ図形を描画する。
	int left = (downPoint.x < point.x) ? downPoint.x : point.x;
	int right = (downPoint.x < point.x) ? point.x : downPoint.x;
	int top = (downPoint.y < point.y) ? downPoint.y : point.y;
	int bottom = (downPoint.y < point.y) ? point.y : downPoint.y;

	// メニューで選択されている図形を生成する
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
	//	左ボタンを押した座標と離した座標を対角線に持つ図形を描画する。
	int left = (downPoint.x < upPoint.x) ? downPoint.x : upPoint.x;
	int right = (downPoint.x < upPoint.x) ? upPoint.x : downPoint.x;
	int top = (downPoint.y < upPoint.y) ? downPoint.y : upPoint.y;
	int bottom = (downPoint.y < upPoint.y) ? upPoint.y : downPoint.y;

	// メニューで選択されている図形を生成する
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

	// 生成した図形をコレクションに追加する。
	shapes.push_back( pShape );
}

void PrintMode(HDC hdc)
{
	static char *pszModeString[] = {
		"楕円の作成",
		"長方形の作成",
		"図形の移動",
		"引っ張り変形",
		"ハートの作成"};

	// 現在のモード表示
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

	// コレクション内のすべての図形を描画する
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
				//　左ボタンを押したときの座標を保持する。
				downPoint = lastPoint = MAKEPOINTS(lParam);	

				// マウス直下の図形を検索する。
				// 見つかった場合は、pActiveShapeに代入し、ドラッグの対象とする。
				// ※見つからなかった場合は、pActiveShape = NULL
				//   -> 新規の図形を追加するモード
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
					MessageBox(hWnd, "モードが選択されていません", "おかしい", MB_OK);
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
			// 再描画
			InvalidateRect(hWnd, NULL, FALSE);
			// メニュー選択の解析:
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

				// メモリDCをクリア
				RECT rect;
				GetClientRect(hWnd, &rect);
				FillRect( hMemDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

				OnPaint(hMemDC);

				// メモリDCを画面に転送
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

// マウス直下の図形を検索する。
CShape* FindActiveShape(POINT point)
{
	for(int i = shapes.size() - 1; i >= 0; i--)
		if(shapes[i]->IsHit(DPoint(point.x, point.y))) return shapes[i];

	return NULL;
}

// バージョン情報ボックス用メッセージ ハンドラ
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

