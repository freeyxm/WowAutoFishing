
// WowFisherDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WowFisher.h"
#include "WowFisherDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <exception>
#include <cstdio>
#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CONFIG_APP  L"Fisher"
#define CONFIG_FILE L".//Fisher.ini"

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CWowFisherDlg �Ի���



CWowFisherDlg::CWowFisherDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWowFisherDlg::IDD, pParent)
	, m_bStart(false), m_pFisher(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWowFisherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWowFisherDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
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


// CWowFisherDlg ��Ϣ�������

BOOL CWowFisherDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	Init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CWowFisherDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CWowFisherDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

static void WritePrivateProfileInt(LPCWSTR key, int value)
{
	WCHAR buf[20];
	wsprintf(buf, L"%d", value);
	WritePrivateProfileString(CONFIG_APP, key, buf, CONFIG_FILE);
}

bool CWowFisherDlg::LoadConfig()
{
	m_nAmpMax = GetPrivateProfileInt(CONFIG_APP, L"AmpMax", 10, CONFIG_FILE);
	m_nAmpH = GetPrivateProfileInt(CONFIG_APP, L"AmpH", 200, CONFIG_FILE);
	m_nAmpL = GetPrivateProfileInt(CONFIG_APP, L"AmpL", 5, CONFIG_FILE);

	m_hotkeyThrow = GetPrivateProfileInt(CONFIG_APP, L"HotkeyThrow", 0x31, CONFIG_FILE);
	m_hotkeyBite1 = GetPrivateProfileInt(CONFIG_APP, L"HotkeyBite1", 0x33, CONFIG_FILE);
	m_hotkeyBite2 = GetPrivateProfileInt(CONFIG_APP, L"HotkeyBite2", 0x34, CONFIG_FILE);
	m_hotkeyBite3 = GetPrivateProfileInt(CONFIG_APP, L"HotkeyBite3", 0x35, CONFIG_FILE);

	m_silentMaxCount = GetPrivateProfileInt(CONFIG_APP, L"SilentMaxCount", 10, CONFIG_FILE);
	m_soundMinCount = GetPrivateProfileInt(CONFIG_APP, L"SoundMinCount", 20, CONFIG_FILE);

	m_bShowConsole = GetPrivateProfileInt(CONFIG_APP, L"ShowConsole", 0, CONFIG_FILE);
	
	return true;
}

void CWowFisherDlg::SaveConfig()
{
	WritePrivateProfileInt(L"AmpMax", m_nAmpMax);
	WritePrivateProfileInt(L"AmpH", m_nAmpH);
	WritePrivateProfileInt(L"AmpL", m_nAmpL);

	m_hotkeyThrow = m_pHotKeyFishing->GetHotKey();
	m_hotkeyBite1 = m_pHotKeyBite1->GetHotKey();
	m_hotkeyBite2 = m_pHotKeyBite2->GetHotKey();
	m_hotkeyBite3 = m_pHotKeyBite3->GetHotKey();

	WritePrivateProfileInt(L"HotkeyThrow", m_hotkeyThrow);
	WritePrivateProfileInt(L"HotkeyBite1", m_hotkeyBite1);
	WritePrivateProfileInt(L"HotkeyBite2", m_hotkeyBite2);
	WritePrivateProfileInt(L"HotkeyBite3", m_hotkeyBite3);

	CString str;
	m_pEditSilentMax->GetWindowTextW(str);
	m_silentMaxCount = _ttoi(str);
	m_pEditSoundMin->GetWindowTextW(str);
	m_soundMinCount = _ttoi(str);

	WritePrivateProfileInt(L"SilentMaxCount", m_silentMaxCount);
	WritePrivateProfileInt(L"SoundMinCount", m_soundMinCount);

	m_bShowConsole = m_pCbConsole->GetCheck();
	WritePrivateProfileInt(L"ShowConsole", m_bShowConsole);
}

void CWowFisherDlg::ApplyConfig()
{
	UpdateAmpMax(m_nAmpMax);

	m_pHotKeyFishing->SetHotKey(m_hotkeyThrow & 0xff, (m_hotkeyThrow >> 8) & 0xff);
	m_pHotKeyBite1->SetHotKey(m_hotkeyBite1 & 0xff, (m_hotkeyBite1 >> 8) & 0xff);
	m_pHotKeyBite2->SetHotKey(m_hotkeyBite2 & 0xff, (m_hotkeyBite2 >> 8) & 0xff);
	m_pHotKeyBite3->SetHotKey(m_hotkeyBite3 & 0xff, (m_hotkeyBite3 >> 8) & 0xff);

	CString str;
	str.Format(L"%d", m_silentMaxCount);
	m_pEditSilentMax->SetWindowTextW(str);
	str.Format(L"%d", m_soundMinCount);
	m_pEditSoundMin->SetWindowTextW(str);

	m_pCbConsole->SetCheck(m_bShowConsole);

	if (m_pFisher != NULL)
	{
		m_pFisher->SetAmpL(m_nAmpL / 100.0f);
		m_pFisher->SetAmpH(m_nAmpH / 100.0f);
		m_pFisher->SetSilentMax(m_silentMaxCount);
		m_pFisher->SetSoundMin(m_soundMinCount);
		m_pFisher->SetHotkeyThrow(m_hotkeyThrow);
		m_pFisher->SetHotkeyBite1(m_hotkeyBite1);
		m_pFisher->SetHotkeyBite2(m_hotkeyBite2);
		m_pFisher->SetHotkeyBite3(m_hotkeyBite3);
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
	m_pTxtThrow = (CStatic*)GetDlgItem(IDC_STATIC_THROW);
	m_pTxtTimeout = (CStatic*)GetDlgItem(IDC_STATIC_TIMEOUT);
	m_pTxtFloat = (CStatic*)GetDlgItem(IDC_STATIC_FLOAT);
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
	::CoInitialize(NULL);

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

	if (m_bShowConsole != 0)
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
		hwnd = ::FindWindowEx(NULL, hwnd, NULL, L"ħ������");
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
	m_pSliderAmpL->SetPos(m_nAmpL * max / m_nAmpMax);
	m_pSliderAmpH->SetPos(m_nAmpH * max / m_nAmpMax);

	m_nAmpMax = max;

	UpdateAmpText(m_pTxtAmpL, m_nAmpL, m_pSliderAmpL->GetRangeMax());
	UpdateAmpText(m_pTxtAmpH, m_nAmpH, m_pSliderAmpH->GetRangeMax());
}

void CWowFisherDlg::UpdateAmpText(CStatic *pStatic, int value, int max)
{
	CString str;
	str.Format(L"%.2lf/%.2lf", (double)value * m_nAmpMax / max, (double)m_nAmpMax);
	pStatic->SetWindowTextW(str);
}

void CWowFisherDlg::OpenConsole()
{
	::setlocale(LC_CTYPE, "");
	if (::AllocConsole())
	{
		FILE *file;
		if (::freopen_s(&file, "CONOUT$", "w", stdout) == 0)
		{
			*stdout = *file;
		}
		if (::freopen_s(&file, "CONIN$", "r", stdin) == 0)
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
	int throwCount = 0;
	int timeoutCount = 0;
	int floatCount = 0;

	if (m_pFisher != NULL)
	{
		throwCount = m_pFisher->GetThrowCount();
		timeoutCount = m_pFisher->GetTimeoutCount();
		floatCount = m_pFisher->GetFindFloatFailCount();
	}

	CString str;
	str.Format(L"%d", throwCount);
	m_pTxtThrow->SetWindowTextW(str);

	str.Format(L"%d", timeoutCount);
	m_pTxtTimeout->SetWindowTextW(str);

	str.Format(L"%d", floatCount);
	m_pTxtFloat->SetWindowTextW(str);

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
		txtStart.LoadStringW(IDS_START);
	if (txtStop.IsEmpty())
		txtStop.LoadStringW(IDS_STOP);

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

void CWowFisherDlg::OnNMCustomdrawSliderAmpL(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	
	int pos = m_pSliderAmpL->GetPos();
	if (pos != m_nAmpL)
	{
		//if (pos > m_nAmpH)
		//{
		//	m_pSliderAmpL->SetPos(m_nAmpH);
		//}
		//else
		{
			m_nAmpL = pos;
			UpdateAmpText(m_pTxtAmpL, m_nAmpL, m_pSliderAmpL->GetRangeMax());
			if (m_pFisher != NULL)
			{
				m_pFisher->SetAmpL(m_nAmpL * m_nAmpMax / 100.0f);
			}
		}
	}

	*pResult = 0;
}

void CWowFisherDlg::OnNMCustomdrawSliderAmpH(NMHDR *pNMHDR, LRESULT *pResult)
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
	if (pos != m_nAmpH)
	{
		m_nAmpH = pos;
		UpdateAmpText(m_pTxtAmpH, m_nAmpH, m_pSliderAmpH->GetRangeMax());
		if (m_pFisher != NULL)
		{
			m_pFisher->SetAmpH((float)m_nAmpH * m_nAmpMax / m_pSliderAmpH->GetRangeMax());
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
		m_pFisher->ResetStatistics();
	}
	OnUpdateStatistics(0, 0);
}


void CWowFisherDlg::OnBnClickedCheckConsole()
{
	m_bShowConsole = m_pCbConsole->GetCheck();
	if (m_bShowConsole == 0)
	{
		CloseConsole();
	}
	else if (m_bShowConsole == 1)
	{
		OpenConsole();
	}
}
