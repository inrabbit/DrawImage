#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;					// ���݂̃C���X�^���X
TCHAR szTitle[MAX_LOADSTRING];				// �^�C�g�� �o�[ �e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// �^�C�g�� �o�[ �e�L�X�g

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̑O�錾:
ATOM			MyRegisterClass( HINSTANCE hInstance );
BOOL			InitInstance( HINSTANCE, int );
LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK	About( HWND, UINT, WPARAM, LPARAM );

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
 	// TODO: ���̈ʒu�ɃR�[�h���L�q���Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o�� �X�g�����O�����������܂�
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAWIMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass( hInstance );

	// �A�v���P�[�V�����̏��������s���܂�:
	if( !InitInstance( hInstance, nCmdShow ) ) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DRAWIMAGE);

	// ���C�� ���b�Z�[�W ���[�v:
	while( GetMessage(&msg, NULL, 0, 0) ) 
	{
		if( !TranslateAccelerator (msg.hwnd, hAccelTable, &msg) ) 
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �p�r: �E�B���h�E �N���X�̓o�^
//
//  �R�����g:
//
//    ���̊֐�����т��̎g�p�͂��̃R�[�h�� Windows 95 �Ő�ɒǉ����ꂽ
//    'RegisterClassEx' �֐��� Win32 �V�X�e���̌݊�����ێ��������ꍇ��
//    �̂ݕK�v�ƂȂ�܂��B�A�v���P�[�V�������A�A�v���P�[�V�����Ɋ֘A�t��
//    ��ꂽ�X���[�� �A�C�R�����擾�ł���悤�A���̊֐����Ăяo�����Ƃ�
//    �d�v�ł��B
//
ATOM MyRegisterClass( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style		= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon		= LoadIcon(hInstance, (LPCTSTR)IDI_DRAWIMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_DRAWIMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx( &wcex );
}

//
//   �֐�: InitInstance(HANDLE, int)
//
//   �p�r: �C���X�^���X �n���h���̕ۑ��ƃ��C�� �E�B���h�E�̍쐬
//
//   �R�����g:
//
//        ���̊֐��ł́A�C���X�^���X �n���h�����O���[�o���ϐ��ɕۑ����A�v���O������
//        ���C�� �E�B���h�E���쐬���\�����܂��B
//
BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X �n���h����ۑ����܂�

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if( !hWnd ) 
   {
      return FALSE;
   }

   ShowWindow( hWnd, nCmdShow );
   UpdateWindow( hWnd );

   return TRUE;
}
