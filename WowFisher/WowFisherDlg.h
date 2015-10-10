
// WowFisherDlg.h : 头文件
//

#pragma once
#include <list>
#include "resource.h"
#include "Fisher.h"

#define WMU_UPDATE_STATISTICS (WM_USER + 100)
#define WMU_UPDATE_STATUS     (WM_USER + 101)

// CWowFisherDlg 对话框
class CWowFisherDlg : public CDialogEx
{
// 构造
public:
	CWowFisherDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WOWFISHER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	bool LoadConfig();
	void SaveConfig();
	void ApplyConfig();

	bool Init();
	bool InitComponents();

	void FindWindows();

	void UpdateAmpMax(int max);
	void UpdateAmpText(CStatic *pStatic, int value, int max);

	LRESULT OnUpdateStatistics(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateStatus(WPARAM wParam, LPARAM lParam);

protected:
	CComboBox* m_pComboBox;
	CButton* m_pBtnRefresh;
	CButton* m_pBtnStart;
	CSliderCtrl* m_pSliderAmpL;
	CSliderCtrl* m_pSliderAmpH;
	CStatic* m_pTxtAmpL;
	CStatic* m_pTxtAmpH;
	CStatic* m_pTxtStatus;
	CStatic* m_pTxtThrow;
	CStatic* m_pTxtTimeout;
	CStatic* m_pTxtFloat;
	CHotKeyCtrl* m_pHotKeyFishing;
	CHotKeyCtrl* m_pHotKeyBite1;
	CHotKeyCtrl* m_pHotKeyBite2;
	CHotKeyCtrl* m_pHotKeyBite3;

	bool m_bStart;
	Fisher* m_pFisher;

	int m_nAmpMax;
	int m_nAmpL;
	int m_nAmpH;

	DWORD m_hotkeyThrow;
	DWORD m_hotkeyBite1;
	DWORD m_hotkeyBite2;
	DWORD m_hotkeyBite3;

	struct WndInfo
	{
		HWND hwnd;
		DWORD pid;
	};
	std::list<WndInfo> m_wnds;

public:
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnNMCustomdrawSliderAmpL(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderAmpH(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonReset();
};
