
// WowFisherDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WowFisher.h"
#include "WowFisherDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <exception>
#include <cstdio>
#include <locale.h>
#include "CommUtil/CommUtil.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CONFIG_FILE L".//config/Fisher.ini"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

    // 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CWowFisherDlg 对话框



CWowFisherDlg::CWowFisherDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CWowFisherDlg::IDD, pParent)
    , m_pComboBox(NULL)
    , m_pBtnRefresh(NULL)
    , m_pBtnStart(NULL)
    , m_pSliderAmpL(NULL)
    , m_pSliderAmpH(NULL)
    , m_pTxtAmpL(NULL)
    , m_pTxtAmpH(NULL)
    , m_pTxtStatus(NULL)
    , m_pTxtThrowCount(NULL)
    , m_pTxtTimeoutCount(NULL)
    , m_pTxtFloatCount(NULL)
    , m_pHotKeyFishing(NULL)
    , m_pHotKeyBite1(NULL)
    , m_pHotKeyBite2(NULL)
    , m_pHotKeyBite3(NULL)
    , m_pEditSilentMax(NULL)
    , m_pEditSoundMin(NULL)
    , m_pCbConsole(NULL)
    , m_bStart(false)
    , m_pFisher(NULL)
    , m_config(CONFIG_FILE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CWowFisherDlg::~CWowFisherDlg()
{
    SAFE_DELETE(m_pFisher);
}

void CWowFisherDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWowFisherDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_MESSAGE(WMU_UPDATE_STATISTICS, &CWowFisherDlg::OnUpdateStatistics)
    ON_MESSAGE(WMU_UPDATE_STATUS, &CWowFisherDlg::OnUpdateStatus)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CWowFisherDlg::OnCbnSelchangeCombo1)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_AMP_L, &CWowFisherDlg::OnNMCustomdrawSliderAmpL)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_AMP_H, &CWowFisherDlg::OnNMCustomdrawSliderAmpH)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CWowFisherDlg::OnBnClickedButtonRefresh)
    ON_BN_CLICKED(IDC_BUTTON_START, &CWowFisherDlg::OnBnClickedButtonStart)
    ON_BN_CLICKED(IDOK, &CWowFisherDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_RESET, &CWowFisherDlg::OnBnClickedButtonReset)
    ON_BN_CLICKED(IDC_CHECK_CONSOLE, &CWowFisherDlg::OnBnClickedCheckConsole)
END_MESSAGE_MAP()


// CWowFisherDlg 消息处理程序

BOOL CWowFisherDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    // TODO:  在此添加额外的初始化代码
    Init();

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWowFisherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWowFisherDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWowFisherDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

afx_msg void CWowFisherDlg::OnDestroy()
{
    SaveConfig();
}

bool CWowFisherDlg::LoadConfig()
{
    return m_config.LoadConfig();
}

void CWowFisherDlg::SaveConfig()
{
    m_config.m_hotkeyThrow = m_pHotKeyFishing->GetHotKey();
    m_config.m_hotkeyBite1 = m_pHotKeyBite1->GetHotKey();
    m_config.m_hotkeyBite2 = m_pHotKeyBite2->GetHotKey();
    m_config.m_hotkeyBite3 = m_pHotKeyBite3->GetHotKey();

    CString str;
    m_pEditSilentMax->GetWindowTextW(str);
    m_config.m_nSilentMaxCount = _ttoi(str);
    m_pEditSoundMin->GetWindowTextW(str);
    m_config.m_nSoundMinCount = _ttoi(str);

    m_config.m_bShowConsole = m_pCbConsole->GetCheck();

    m_config.SaveConfig();
}

void CWowFisherDlg::ApplyConfig()
{
    UpdateAmpMax(m_config.m_nAmpMax);

    m_pHotKeyFishing->SetHotKey(m_config.m_hotkeyThrow & 0xff, (m_config.m_hotkeyThrow >> 8) & 0xff);
    m_pHotKeyBite1->SetHotKey(m_config.m_hotkeyBite1 & 0xff, (m_config.m_hotkeyBite1 >> 8) & 0xff);
    m_pHotKeyBite2->SetHotKey(m_config.m_hotkeyBite2 & 0xff, (m_config.m_hotkeyBite2 >> 8) & 0xff);
    m_pHotKeyBite3->SetHotKey(m_config.m_hotkeyBite3 & 0xff, (m_config.m_hotkeyBite3 >> 8) & 0xff);

    CString str;
    str.Format(L"%d", m_config.m_nSilentMaxCount);
    m_pEditSilentMax->SetWindowTextW(str);
    str.Format(L"%d", m_config.m_nSoundMinCount);
    m_pEditSoundMin->SetWindowTextW(str);

    m_pCbConsole->SetCheck(m_config.m_bShowConsole);

    if (m_pFisher != NULL)
    {
        m_pFisher->SetAmpL(m_config.m_nAmpL / 100.0f);
        m_pFisher->SetAmpH(m_config.m_nAmpH / 100.0f);
        m_pFisher->SetSilentMax(m_config.m_nSilentMaxCount);
        m_pFisher->SetSoundMin(m_config.m_nSoundMinCount);
        m_pFisher->SetHotkeyThrow(m_config.m_hotkeyThrow);
        m_pFisher->SetHotkeyBite1(m_config.m_hotkeyBite1);
        m_pFisher->SetHotkeyBite2(m_config.m_hotkeyBite2);
        m_pFisher->SetHotkeyBite3(m_config.m_hotkeyBite3);
        m_pFisher->SetThrowCount(m_config.m_nThrowCount);
        m_pFisher->SetTimeoutCount(m_config.m_nTimeoutCount);
        m_pFisher->SetFindFloatFailCount(m_config.m_nFloatCount);
    }
}

bool CWowFisherDlg::InitComponents()
{
    m_pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO1);
    m_pBtnRefresh = (CButton*)GetDlgItem(IDC_BUTTON_REFRESH);
    m_pBtnStart = (CButton*)GetDlgItem(IDC_BUTTON_START);
    m_pSliderAmpL = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_AMP_L);
    m_pSliderAmpH = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_AMP_H);
    m_pTxtAmpL = (CStatic*)GetDlgItem(IDC_TXT_AMP_L);
    m_pTxtAmpH = (CStatic*)GetDlgItem(IDC_TXT_AMP_H);
    m_pTxtStatus = (CStatic*)GetDlgItem(IDC_STATIC_STATUS);
    m_pTxtThrowCount = (CStatic*)GetDlgItem(IDC_STATIC_THROW);
    m_pTxtTimeoutCount = (CStatic*)GetDlgItem(IDC_STATIC_TIMEOUT);
    m_pTxtFloatCount = (CStatic*)GetDlgItem(IDC_STATIC_FLOAT);
    m_pHotKeyFishing = (CHotKeyCtrl*)GetDlgItem(IDC_HOTKEY_FISHING);
    m_pHotKeyBite1 = (CHotKeyCtrl*)GetDlgItem(IDC_HOTKEY_BAIT_1);
    m_pHotKeyBite2 = (CHotKeyCtrl*)GetDlgItem(IDC_HOTKEY_BAIT_2);
    m_pHotKeyBite3 = (CHotKeyCtrl*)GetDlgItem(IDC_HOTKEY_BAIT_3);
    m_pEditSilentMax = (CEdit*)GetDlgItem(IDC_EDIT_SILENT_MIN);
    m_pEditSoundMin = (CEdit*)GetDlgItem(IDC_EDIT_SOUND_MAX);
    m_pCbConsole = (CButton*)GetDlgItem(IDC_CHECK_CONSOLE);

    m_pBtnStart->EnableWindow(FALSE);

    return true;
}

bool CWowFisherDlg::Init()
{
    int ret = ::CoInitialize(NULL);
    if (ret != 0)
    {
        return false;
    }

    LoadConfig();
    InitComponents();

    int x = 200, y = 20;
    int w = 600, h = 400;
    m_pFisher = new Fisher(NULL, x, y, w, h);
    if (m_pFisher == NULL)
    {
        MessageBox(L"Fisher new failed");
        return false;
    }

    if (!m_pFisher->Init())
    {
        MessageBox(L"Fisher init failed");
        return false;
    }

    if (m_config.m_bShowConsole != 0)
    {
        OpenConsole();
    }

    ApplyConfig();
    OnUpdateStatistics(0, 0);

    return true;
}

void CWowFisherDlg::FindWindows()
{
    m_wnds.clear();
    m_pComboBox->ResetContent();

    HWND hwnd = NULL;
    DWORD pid;
    do
    {
        hwnd = ::FindWindowEx(NULL, hwnd, NULL, L"魔兽世界");
        if (hwnd != NULL)
        {
            DWORD tid = ::GetWindowThreadProcessId(hwnd, &pid);
            if (tid == 0)
            {
                printf("GetWindowThreadProcessId faield.\n");
                continue;
            }
            else
            {
                WndInfo info;
                info.hwnd = hwnd;
                info.pid = pid;
                m_wnds.push_back(info);

                CString str;
                str.Format(L"%d", pid);
                m_pComboBox->AddString(str);
            }
        }
    } while (hwnd != NULL);

    if (m_pComboBox->GetCount() > 0)
    {
        m_pComboBox->SetCurSel(0);
    }
    OnCbnSelchangeCombo1();
}

void CWowFisherDlg::UpdateAmpMax(int max)
{
    m_pSliderAmpL->SetRange(0, max * 100);
    m_pSliderAmpH->SetRange(0, max * 100);
    m_pSliderAmpL->SetPos(m_config.m_nAmpL * max / m_config.m_nAmpMax);
    m_pSliderAmpH->SetPos(m_config.m_nAmpH * max / m_config.m_nAmpMax);

    m_config.m_nAmpMax = max;

    UpdateAmpText(m_pTxtAmpL, m_config.m_nAmpL, m_pSliderAmpL->GetRangeMax());
    UpdateAmpText(m_pTxtAmpH, m_config.m_nAmpH, m_pSliderAmpH->GetRangeMax());
}

void CWowFisherDlg::UpdateAmpText(CStatic* pStatic, int value, int max)
{
    CString str;
    str.Format(L"%.2lf/%.2lf", (double)value * m_config.m_nAmpMax / max, (double)m_config.m_nAmpMax);
    pStatic->SetWindowTextW(str);
}

void CWowFisherDlg::OpenConsole()
{
    ::setlocale(LC_CTYPE, "");
    if (::AllocConsole())
    {
        FILE* file;
        if (::freopen_s(&file, "CONOUT$", "w", stdout) == 0 && file)
        {
            *stdout = *file;
        }
        if (::freopen_s(&file, "CONIN$", "r", stdin) == 0 && file)
        {
            *stdin = *file;
        }
    }
}

void CWowFisherDlg::CloseConsole()
{
    ::FreeConsole();
}

LRESULT CWowFisherDlg::OnUpdateStatistics(WPARAM wParam, LPARAM lParam)
{
    if (m_pFisher != NULL)
    {
        m_config.m_nThrowCount = m_pFisher->GetThrowCount();
        m_config.m_nTimeoutCount = m_pFisher->GetTimeoutCount();
        m_config.m_nFloatCount = m_pFisher->GetFindFloatFailCount();
    }

    CString str;
    str.Format(L"%d", m_config.m_nThrowCount);
    m_pTxtThrowCount->SetWindowTextW(str);

    str.Format(L"%d", m_config.m_nTimeoutCount);
    m_pTxtTimeoutCount->SetWindowTextW(str);

    str.Format(L"%d", m_config.m_nFloatCount);
    m_pTxtFloatCount->SetWindowTextW(str);

    return 0;
}

LRESULT CWowFisherDlg::OnUpdateStatus(WPARAM wParam, LPARAM lParam)
{
    LPCWSTR msg = (LPCWSTR)wParam;
    m_pTxtStatus->SetWindowTextW(msg);

    return 0;
}

void CWowFisherDlg::OnCbnSelchangeCombo1()
{
    int index = m_pComboBox->GetCurSel();
    m_pBtnStart->EnableWindow(index != CB_ERR);
}

void CWowFisherDlg::OnBnClickedButtonRefresh()
{
    FindWindows();
}

void CWowFisherDlg::OnBnClickedButtonStart()
{
    static CString txtStart, txtStop;
    if (txtStart.IsEmpty())
    {
        bool res = txtStart.LoadStringW(IDS_START);
        if (!res) txtStart = "start";
    }
    if (txtStop.IsEmpty())
    {
        bool res = txtStop.LoadStringW(IDS_STOP);
        if (!res) txtStop = "stop";
    }

    m_bStart = !m_bStart;
    m_pBtnStart->SetWindowTextW(m_bStart ? txtStop : txtStart);
    m_pBtnRefresh->EnableWindow(!m_bStart);
    m_pComboBox->EnableWindow(!m_bStart);

    if (m_bStart)
    {
        int index = m_pComboBox->GetCurSel();
        if (index >= 0 && index < (int)m_wnds.size())
        {
            std::list<WndInfo>::iterator it = m_wnds.begin();
            for (int i = 0; i < index; ++i)
            {
                ++it;
            }
            if (m_pFisher != NULL)
            {
                m_pFisher->SetMainHWnd(GetSafeHwnd());
                m_pFisher->SetWowHWnd(it->hwnd);
                m_pFisher->Start();
            }
        }
    }
    else
    {
        if (m_pFisher != NULL)
        {
            m_pFisher->Stop();
            m_pTxtStatus->SetWindowTextW(L"");
        }
    }
}

void CWowFisherDlg::OnNMCustomdrawSliderAmpL(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here

    int pos = m_pSliderAmpL->GetPos();
    if (pos != m_config.m_nAmpL)
    {
        //if (pos > m_nAmpH)
        //{
        //	m_pSliderAmpL->SetPos(m_nAmpH);
        //}
        //else
        {
            m_config.m_nAmpL = pos;
            UpdateAmpText(m_pTxtAmpL, m_config.m_nAmpL, m_pSliderAmpL->GetRangeMax());
            if (m_pFisher != NULL)
            {
                m_pFisher->SetAmpL(m_config.m_nAmpL * m_config.m_nAmpMax / 100.0f);
            }
        }
    }

    *pResult = 0;
}

void CWowFisherDlg::OnNMCustomdrawSliderAmpH(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO: Add your control notification handler code here

    int pos = m_pSliderAmpH->GetPos();
    //if (pos >= m_pSliderAmpH->GetRangeMax() && pos == m_nAmpH)
    //{
    //	UpdateAmpMax(m_nAmpMax * 10);
    //}
    //else if (pos <= m_pSliderAmpH->GetRangeMin() && pos == m_nAmpH)
    //{
    //	UpdateAmpMax(m_nAmpMax / 10);
    //}
    //else 
    if (pos != m_config.m_nAmpH)
    {
        m_config.m_nAmpH = pos;
        UpdateAmpText(m_pTxtAmpH, m_config.m_nAmpH, m_pSliderAmpH->GetRangeMax());
        if (m_pFisher != NULL)
        {
            m_pFisher->SetAmpH((float)m_config.m_nAmpH * m_config.m_nAmpMax / m_pSliderAmpH->GetRangeMax());
        }
    }

    *pResult = 0;
}


void CWowFisherDlg::OnBnClickedOk()
{
    SaveConfig();
    ApplyConfig();
    //CDialogEx::OnOK();
}


void CWowFisherDlg::OnBnClickedButtonReset()
{
    if (m_pFisher != NULL)
    {
        m_config.m_nThrowCount = m_config.m_nFloatCount = m_config.m_nTimeoutCount = 0;
        m_pFisher->SetThrowCount(m_config.m_nThrowCount);
        m_pFisher->SetFindFloatFailCount(m_config.m_nFloatCount);
        m_pFisher->SetTimeoutCount(m_config.m_nTimeoutCount);
    }
    OnUpdateStatistics(0, 0);
}


void CWowFisherDlg::OnBnClickedCheckConsole()
{
    m_config.m_bShowConsole = m_pCbConsole->GetCheck();
    if (m_config.m_bShowConsole == 0)
    {
        CloseConsole();
    }
    else if (m_config.m_bShowConsole == 1)
    {
        OpenConsole();
    }
}
