// WaveGraph.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "WaveGraph.h"
#include "AudioRecorder.h"
#include "AudioRenderer.h"
#include <cstdio>

#define MAX_LOADSTRING 100

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define START_CAPTURE_ID    200
#define STOP_CAPTURE_ID     201
#define START_PLAY_ID       202
#define STOP_PLAY_ID        203
#define ADD_SCALE_ID        204
#define SUB_SCALE_ID        205

#define TIMER_ID_SOUND 0x110
UINT g_soundTimer = 0;

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
HWND g_hWndMain;
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

static AudioRecorder *g_pAudioRecorder = NULL;
static AudioRenderer *g_pAudioRenderer = NULL;


// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VOID StartRecord();
VOID StopRecord();
VOID StartPlay();
VOID StopPlay();
VOID AddScale();
VOID SubScale();


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WAVEGRAPH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WAVEGRAPH));

	::CoInitialize(NULL);

	if (::AllocConsole())
	{
		::freopen("CONOUT$", "w", stdout);
	}
	printf("Start ...\n");

	g_soundTimer = ::SetTimer(g_hWndMain, TIMER_ID_SOUND, 100, NULL);

	g_pAudioRecorder = new AudioRecorder();
	if (!g_pAudioRecorder || FAILED(g_pAudioRecorder->Init()))
		return FALSE;

	g_pAudioRenderer = new AudioRenderer();
	if (!g_pAudioRenderer || FAILED(g_pAudioRenderer->Init()))
		return FALSE;

	// ����Ϣѭ��:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	::CoUninitialize();
	::FreeConsole();

	return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WAVEGRAPH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WAVEGRAPH);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

	int cx = GetSystemMetrics( SM_CXSCREEN ) / 2 - WINDOW_WIDTH / 2;
	int cy = GetSystemMetrics( SM_CYSCREEN ) / 2 - WINDOW_HEIGHT / 2;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		cx, cy, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_hWndMain = hWnd;

	return TRUE;
}

VOID CreateControlButtons(HWND hWndParent)
{
	const INT nButtonWidth = 100;
	const INT nButtonHeight = 30;
	const DWORD dwButtonStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT;

	HWND hWndStartRecord = CreateWindow(_T("BUTTON"), _T("Start Capture"), dwButtonStyle,
		20, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU)START_CAPTURE_ID, hInst, NULL);

	HWND hWndStopRecord = CreateWindow(_T("BUTTON"), _T("Stop Capture"), dwButtonStyle,
		150, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU)STOP_CAPTURE_ID, hInst, NULL);

	HWND hWndStartRender = CreateWindow(_T("BUTTON"), _T("Start Play"), dwButtonStyle,
		20, 60, nButtonWidth, nButtonHeight, hWndParent, (HMENU)START_PLAY_ID, hInst, NULL);

	HWND hWndStopRender = CreateWindow(_T("BUTTON"), _T("Stop Play"), dwButtonStyle,
		150, 60, nButtonWidth, nButtonHeight, hWndParent, (HMENU)STOP_PLAY_ID, hInst, NULL);

	HWND hWndStartPlay = CreateWindow(_T("BUTTON"), _T("+Scale"), dwButtonStyle,
		280, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU)ADD_SCALE_ID, hInst, NULL);

	HWND hWndStopPlay = CreateWindow(_T("BUTTON"), _T("-Scale"), dwButtonStyle,
		410, 20, nButtonWidth, nButtonHeight, hWndParent, (HMENU)SUB_SCALE_ID, hInst, NULL);
}

VOID UpdateButtonStatus(BOOL bEnableStartCapture, BOOL bEnableStopCapture, BOOL bEnableStartPlay, BOOL bEnableStopPlay)
{
	HWND hWndStartRecord = GetDlgItem(g_hWndMain, START_CAPTURE_ID);
	HWND hWndStopRecord = GetDlgItem(g_hWndMain, STOP_CAPTURE_ID);
	HWND hWndStartPlay = GetDlgItem(g_hWndMain, START_PLAY_ID);
	HWND hWndStopPlay = GetDlgItem(g_hWndMain, STOP_PLAY_ID);
	HWND hWndAddScale = GetDlgItem(g_hWndMain, ADD_SCALE_ID);
	HWND hWndSubScale = GetDlgItem(g_hWndMain, SUB_SCALE_ID);

	EnableWindow(hWndStartRecord, bEnableStartCapture);
	EnableWindow(hWndStopRecord, bEnableStopCapture);
	EnableWindow(hWndStartPlay, bEnableStartPlay);
	EnableWindow(hWndStopPlay, bEnableStopPlay);
}

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		{
			g_hWndMain = hWnd;
			CreateControlButtons(hWnd);
			UpdateButtonStatus(TRUE, FALSE, FALSE, FALSE);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case START_CAPTURE_ID:
			StartRecord();
			break;
		case STOP_CAPTURE_ID:
			StopRecord();
			break;
		case START_PLAY_ID:
			StartPlay();
			break;
		case STOP_PLAY_ID:
			StopPlay();
			break;
		case ADD_SCALE_ID:
			AddScale();
			break;
		case SUB_SCALE_ID:
			SubScale();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...

		g_pAudioRecorder->Paint(hWnd, hdc);

		wchar_t buf[20];
		wsprintf(buf, L"Scale: %d%%", (int)(g_pAudioRecorder->GetScale() * 100));
		::TextOut(hdc, 0, 0, buf, ::wcslen(buf));

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		{
			RECT rect = { 0, 0,  WINDOW_WIDTH, WINDOW_HEIGHT };
			::InvalidateRect(hWnd, &rect, true);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


VOID StartRecord()
{
	if (g_pAudioRecorder->StartRecord())
	{
		UpdateButtonStatus(FALSE, TRUE, FALSE, FALSE);
	}
}

VOID StopRecord()
{
	g_pAudioRecorder->StopRecord();
	UpdateButtonStatus(TRUE, FALSE, TRUE, TRUE);
}

VOID StartPlay()
{
	g_pAudioRenderer->SetSource(g_pAudioRecorder->GetStorage());
	if (g_pAudioRenderer->StartRender())
	{
		UpdateButtonStatus(FALSE, FALSE, FALSE, TRUE);
	}
}

void StopPlay()
{
	g_pAudioRenderer->StopRender();
	UpdateButtonStatus(TRUE, TRUE, TRUE, FALSE);
}

VOID AddScale()
{
	g_pAudioRecorder->AddScale(+0.5f);
}

VOID SubScale()
{
	g_pAudioRecorder->AddScale(-0.5f);
}
