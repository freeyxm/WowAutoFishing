// WaveGraph.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "WaveGraph.h"
#include "Win32Util/Audio/Extend/AudioRecorder.h"
#include "Win32Util/Audio/Extend/AudioRenderer.h"
#include "Win32Util/Audio/Extend/AudioExtractor.h"
#include "Win32Util/Audio/Extend/AudioPainter.h"
#include "Win32Util/Util/DcBuffer.h"
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
UINT_PTR g_soundTimer = 0;


enum class ActionMode
{
	Mode_None,
	Mode_Record,
	Mode_Play,
	Mode_Extract,
};
static ActionMode g_mode = ActionMode::Mode_None;

// 全局变量:
static HINSTANCE hInst;								// 当前实例
static HWND g_hWndMain;
static TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
static TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

static RECT g_waveRect = { 0, 0, 500, 200 };
static DcBuffer *g_pDcBuffer;

static AudioRecorder *g_pAudioRecorder = NULL;
static AudioRenderer *g_pAudioRenderer = NULL;
static AudioExtractor *g_pAudioExtractor = NULL;
static AudioPainter *g_pAudioPainter = NULL;


// 此代码模块中包含的函数的前向声明:
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
VOID ClickRecorder();
VOID ClickExtractor();

static bool InitGlobal();
static void Paint(HWND hWnd, HDC hdc);
static void PaintExtractor();
static void PaintRecorder();


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WAVEGRAPH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

    if (::AllocConsole())
    {
        FILE* out;
        FILE* in;
        ::freopen_s(&out, "CONOUT$", "w", stdout);
        ::freopen_s(&in, "CONIN$", "r", stdin);
    }
    _wsetlocale(LC_ALL, L"chs");
    printf("Start ...\n");

    if (!InitGlobal())
    {
        printf("Init failed!\n");
        return FALSE;
    }

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WAVEGRAPH));

	// 主消息循环:
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

bool InitGlobal()
{
    int hr = ::CoInitialize(NULL);
    if (hr != S_OK)
    {
        return FALSE;
    }

    if (g_soundTimer == NULL)
    {
        g_soundTimer = ::SetTimer(g_hWndMain, TIMER_ID_SOUND, 20, NULL);
    }

    if (g_pAudioPainter == NULL)
    {
        g_pAudioPainter = new AudioPainter();
        if (!g_pAudioPainter)
            return FALSE;
        g_pAudioPainter->SetEnable(false);
    }

    if (g_pAudioRecorder == NULL)
    {
        g_pAudioRecorder = new AudioRecorder(true, false);
        if (!g_pAudioRecorder || g_pAudioRecorder->Init() != S_OK)
        {
            printf("Init AudioRecorder failed!\n");
            return FALSE;
        }
    }

    if (g_pAudioRenderer == NULL)
    {
        g_pAudioRenderer = new AudioRenderer(false);
        if (!g_pAudioRenderer || g_pAudioRenderer->Init() != S_OK)
        {
            printf("Init AudioRenderer failed!\n");
            return FALSE;
        }
    }

    if (g_pAudioExtractor == NULL)
    {
        g_pAudioExtractor = new AudioExtractor();
        if (!g_pAudioExtractor || g_pAudioExtractor->Init() != S_OK)
        {
            printf("Init AudioExtractor failed!\n");
            return FALSE;
        }
        g_pAudioExtractor->SetSegmentMaxCount(100);
        g_pAudioExtractor->SetSoundMaxCount(1);
        g_pAudioExtractor->SetAmpZcr(480, 0.1f, 0.2f, 0.3f, 0.5f);
    }

    return true;
}


//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
	wcex.hbrBackground	= NULL; // (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WAVEGRAPH);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	int cx = GetSystemMetrics( SM_CXSCREEN ) / 2 - WINDOW_WIDTH / 2;
	int cy = GetSystemMetrics( SM_CYSCREEN ) / 2 - WINDOW_HEIGHT / 2;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		cx, cy, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	g_pDcBuffer = new DcBuffer(hWnd, g_waveRect.right, g_waveRect.bottom);

	ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);

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

	EnableWindow(hWndStartRecord, g_mode == ActionMode::Mode_None);
	EnableWindow(hWndStopRecord, g_mode == ActionMode::Mode_Record);
	
	EnableWindow(hWndStartPlay, g_mode == ActionMode::Mode_None);
	EnableWindow(hWndStopPlay,  g_mode == ActionMode::Mode_Play);

	EnableWindow(hWndStartExtract, g_mode == ActionMode::Mode_None);
	EnableWindow(hWndStopExtract, g_mode == ActionMode::Mode_Extract);
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
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
		// 分析菜单选择:
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
			ClickRecorder();
			break;
		case PAINT_EXTRACT_ID:
			ClickExtractor();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...

		Paint(hWnd, hdc);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		if (g_pAudioPainter->IsEnable())
		{
			if (g_mode == ActionMode::Mode_Extract)
			{
				PaintExtractor();
			}
			else if (g_mode == ActionMode::Mode_Record)
			{
				PaintRecorder();
			}
			RECT rect = { 0, 100,  WINDOW_WIDTH, WINDOW_HEIGHT };
			::InvalidateRect(hWnd, &rect, true);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
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
	if (g_pAudioRecorder->Start())
	{
		g_mode = ActionMode::Mode_Record;
		UpdateButtonStatus();
	}
}

VOID StopRecord()
{
	g_mode = ActionMode::Mode_None;
	g_pAudioRecorder->Stop();
	UpdateButtonStatus();
}

VOID StartExtract()
{
	if (g_pAudioExtractor->Start())
	{
		g_mode = ActionMode::Mode_Extract;
		UpdateButtonStatus();
	}
}

VOID StopExtract()
{
	g_mode = ActionMode::Mode_None;
	g_pAudioExtractor->Stop();
	UpdateButtonStatus();
}

VOID StartPlay()
{
	g_pAudioRenderer->SetSource(g_pAudioRecorder->GetStorage());
	if (g_pAudioRenderer->Start())
	{
		g_mode = ActionMode::Mode_Play;
		UpdateButtonStatus();
	}
}

void StopPlay()
{
	g_mode = ActionMode::Mode_None;
	g_pAudioRenderer->Stop();
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

VOID ClickRecorder()
{
	static bool enable = false;
	enable = !enable;
	if (enable)
	{
		g_pAudioPainter->SetFormat(g_pAudioExtractor->GetFormat());
	}
	g_pAudioPainter->SetEnable(enable);
}

VOID ClickExtractor()
{
	static bool enable = false;
	enable = !enable;
	if (enable)
	{
		g_pAudioPainter->SetFormat(g_pAudioRecorder->GetFormat());
	}
	g_pAudioPainter->SetEnable(enable);
}

void Paint(HWND hWnd, HDC hdc)
{
	if (g_pAudioPainter->IsEnable())
	{
		int w = g_waveRect.right;
		int h = g_waveRect.bottom;
		::StretchBlt(hdc, 10, 100, w, h, g_pDcBuffer->GetDC(), 0, 0, w, h, SRCCOPY);
	}

	wchar_t buf[20];
	wsprintf(buf, L"Scale: %d%%", (int)(g_pAudioPainter->GetScale() * 100));
	::TextOut(hdc, 0, 0, buf, (int)::wcslen(buf));
}

void PaintRecorder()
{
	g_pDcBuffer->Clear();
	g_pAudioPainter->Clear();

	g_pAudioRecorder->Lock();
	g_pAudioPainter->AddSource(g_pAudioRecorder->GetStorage());
	g_pAudioPainter->Paint(g_pDcBuffer->GetDC(), g_waveRect, 10.0f);
	g_pAudioRecorder->Unlock();

	g_pAudioPainter->Clear();
}

void PaintExtractor()
{
	g_pDcBuffer->Clear();
	g_pAudioPainter->Clear();

	g_pAudioExtractor->Lock();
	for (AudioExtractor::SegmentCIter iter = g_pAudioExtractor->cbegin(); iter != g_pAudioExtractor->cend(); ++iter)
	{
		g_pAudioPainter->AddSource(*iter);
	}
	g_pAudioPainter->Paint(g_pDcBuffer->GetDC(), g_waveRect, 10.0f);
	g_pAudioExtractor->Unlock();

	g_pAudioPainter->Clear();
}

