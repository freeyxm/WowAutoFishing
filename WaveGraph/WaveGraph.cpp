// WaveGraph.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "WaveGraph.h"
#include "AudioRecorder.h"
#include "AudioRenderer.h"
#include "AudioExtractor.h"
#include "AudioPainter.h"
#include "WaveCreator.h"
#include <cstdio>

#define MAX_LOADSTRING 100

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

#define START_CAPTURE_ID    200
#define STOP_CAPTURE_ID     201
#define START_PLAY_ID       202
#define STOP_PLAY_ID        203
#define START_EXTRACT_ID    204
#define STOP_EXTRACT_ID     205

#define PAINT_RECORD_ID     300
#define PAINT_EXTRACT_ID    301
#define ADD_SCALE_ID        310
#define SUB_SCALE_ID        311

#define TIMER_ID_SOUND      999
UINT g_soundTimer = 0;


enum Mode
{
	Mode_None,
	Mode_Record,
	Mode_Play,
	Mode_Extract,
};
static Mode g_mode = Mode_None;

// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
HWND g_hWndMain;
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

static AudioRecorder *g_pAudioRecorder = NULL;
static AudioRenderer *g_pAudioRenderer = NULL;
static AudioExtractor *g_pAudioExtractor = NULL;
static AudioPainter *g_pAudioPainter = NULL;


// �˴���ģ���а����ĺ�����ǰ������:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

VOID StartRecord();
VOID StopRecord();

VOID StartPlay();
VOID StopPlay();

VOID StartExtract();
VOID StopExtract();

VOID AddScale();
VOID SubScale();
VOID PaintRecorder();
VOID PaintExtractor();


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

	g_pAudioPainter = new AudioPainter();
	if (!g_pAudioPainter)
		return FALSE;
	g_pAudioPainter->SetEnable(false);

	g_pAudioRecorder = new AudioRecorder();
	if (!g_pAudioRecorder || FAILED(g_pAudioRecorder->Init()))
	{
		printf("Init AudioRecorder failed!\n");
		return FALSE;
	}

	g_pAudioRenderer = new AudioRenderer();
	if (!g_pAudioRenderer || FAILED(g_pAudioRenderer->Init()))
	{
		printf("Init AudioRenderer failed!\n");
		return FALSE;
	}

	g_pAudioExtractor = new AudioExtractor();
	if(!g_pAudioExtractor || FAILED(g_pAudioExtractor->Init()))
	{
		printf("Init AudioExtractor failed!\n");
		return FALSE;
	}

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
	const INT w = 100;
	const INT h = 30;
	const DWORD dwButtonStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT;

	int x = 20;
	int y = 20;
	int w2 = w + 10;
	int h2 = h + 10;

	int c = -1;

	++c;
	HWND hWndStartPlay = CreateWindow(_T("BUTTON"), _T("+Scale"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 0), w, h, hWndParent, (HMENU)ADD_SCALE_ID, hInst, NULL);
	HWND hWndStopPlay = CreateWindow(_T("BUTTON"), _T("-Scale"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 1), w, h, hWndParent, (HMENU)SUB_SCALE_ID, hInst, NULL);

	++c;
	HWND hWndStartRecord = CreateWindow(_T("BUTTON"), _T("Start Record"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 0), w, h, hWndParent, (HMENU)START_CAPTURE_ID, hInst, NULL);
	HWND hWndStopRecord = CreateWindow(_T("BUTTON"), _T("Stop Record"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 1), w, h, hWndParent, (HMENU)STOP_CAPTURE_ID, hInst, NULL);

	++c;
	HWND hWndStartRender = CreateWindow(_T("BUTTON"), _T("Start Play"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 0), w, h, hWndParent, (HMENU)START_PLAY_ID, hInst, NULL);
	HWND hWndStopRender = CreateWindow(_T("BUTTON"), _T("Stop Play"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 1), w, h, hWndParent, (HMENU)STOP_PLAY_ID, hInst, NULL);

	++c;
	HWND hWndStartExtract = CreateWindow(_T("BUTTON"), _T("Start Extract"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 0), w, h, hWndParent, (HMENU)START_EXTRACT_ID, hInst, NULL);
	HWND hWndStopExtract = CreateWindow(_T("BUTTON"), _T("Stop Extract"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 1), w, h, hWndParent, (HMENU)STOP_EXTRACT_ID, hInst, NULL);

	++c;
	HWND hWndPaintRecord = CreateWindow(_T("BUTTON"), _T("Paint Record"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 0), w, h, hWndParent, (HMENU)PAINT_RECORD_ID, hInst, NULL);
	HWND hWndPaintExtract = CreateWindow(_T("BUTTON"), _T("Paint Extract"), dwButtonStyle,
		(x + w2 * c), (y + h2 * 1), w, h, hWndParent, (HMENU)PAINT_EXTRACT_ID, hInst, NULL);
}

VOID UpdateButtonStatus()
{
	HWND hWndStartRecord = GetDlgItem(g_hWndMain, START_CAPTURE_ID);
	HWND hWndStopRecord = GetDlgItem(g_hWndMain, STOP_CAPTURE_ID);

	HWND hWndStartPlay = GetDlgItem(g_hWndMain, START_PLAY_ID);
	HWND hWndStopPlay = GetDlgItem(g_hWndMain, STOP_PLAY_ID);

	HWND hWndStartExtract = GetDlgItem(g_hWndMain, START_EXTRACT_ID);
	HWND hWndStopExtract = GetDlgItem(g_hWndMain, STOP_EXTRACT_ID);

	EnableWindow(hWndStartRecord, g_mode == Mode_None);
	EnableWindow(hWndStopRecord, g_mode == Mode_Record);
	
	EnableWindow(hWndStartPlay, g_mode == Mode_None);
	EnableWindow(hWndStopPlay,  g_mode == Mode_Play);

	EnableWindow(hWndStartExtract, g_mode == Mode_None);
	EnableWindow(hWndStopExtract, g_mode == Mode_Extract);
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
			UpdateButtonStatus();
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
		case START_EXTRACT_ID:
			StartExtract();
			break;
		case STOP_EXTRACT_ID:
			StopExtract();
			break;
		case ADD_SCALE_ID:
			AddScale();
			break;
		case SUB_SCALE_ID:
			SubScale();
			break;
		case PAINT_RECORD_ID:
			PaintRecorder();
			break;
		case PAINT_EXTRACT_ID:
			PaintExtractor();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �ڴ���������ͼ����...

		if (g_pAudioPainter->IsEnable())
		{
			RECT rect;
			::GetWindowRect(hWnd, &rect);

			rect.right = rect.right - rect.left - 20;
			rect.left = 10;
			rect.bottom = (rect.bottom - rect.top) / 2;
			rect.top = 200;

			g_pAudioPainter->Paint(hWnd, hdc, rect, 10.0f);
		}

		wchar_t buf[20];
		wsprintf(buf, L"Scale: %d%%", (int)(g_pAudioPainter->GetScale() * 100));
		::TextOut(hdc, 0, 0, buf, ::wcslen(buf));

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		if (g_pAudioPainter->IsEnable())
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
		g_mode = Mode_Record;
		UpdateButtonStatus();
	}
}

VOID StopRecord()
{
	g_mode = Mode_None;
	g_pAudioRecorder->StopRecord();
	UpdateButtonStatus();
}

VOID StartExtract()
{
	if (g_pAudioExtractor->StartExtract())
	{
		g_mode = Mode_Extract;
		UpdateButtonStatus();
	}
}

VOID StopExtract()
{
	g_mode = Mode_None;
	g_pAudioExtractor->StopExtract();
	UpdateButtonStatus();
}

VOID StartPlay()
{
	g_pAudioRenderer->SetSource(g_pAudioRecorder->GetStorage());
	if (g_pAudioRenderer->StartRender())
	{
		g_mode = Mode_Play;
		UpdateButtonStatus();
	}
}

void StopPlay()
{
	g_mode = Mode_None;
	g_pAudioRenderer->StopRender();
	UpdateButtonStatus();
}

VOID AddScale()
{
	g_pAudioPainter->AddScale(+0.5f);
	
	RECT rect = { 0, 0, 100, 20 };
	::InvalidateRect(g_hWndMain, &rect, true);
}

VOID SubScale()
{
	g_pAudioPainter->AddScale(-0.5f);

	RECT rect = { 0, 0, 100, 20 };
	::InvalidateRect(g_hWndMain, &rect, true);
}

VOID PaintRecorder()
{
	static bool enable = false;
	enable = !enable;
	if (enable)
	{
		g_pAudioPainter->Clear();
		g_pAudioPainter->SetFormat(g_pAudioRecorder->GetFormat());
		g_pAudioPainter->AddSource(g_pAudioRecorder->GetStorage());
	}
	g_pAudioPainter->SetEnable(enable);
}

VOID PaintExtractor()
{
	static bool enable = false;
	enable = !enable;
	if (enable)
	{
		g_pAudioPainter->Clear();
		g_pAudioPainter->SetFormat(g_pAudioExtractor->GetFormat());
		for(AudioExtractor::SegmentCIter iter = g_pAudioExtractor->cbegin(); iter != g_pAudioExtractor->cend(); ++iter)
		{
			g_pAudioPainter->AddSource(*iter);
		}
	}
	g_pAudioPainter->SetEnable(enable);
}

