
// MFCApplication4Dlg.h : header file
//

#pragma once


// CMFCApplication4Dlg dialog
class CMFCApplication4Dlg : public CDialogEx
{
// Construction
public:
	CMFCApplication4Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MFCAPPLICATION4_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	ULONG_PTR m_gdiplusToken;
		HTHUMBNAIL thumbnail ;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedSearch();

// Var
public:
	HWND wndTarget;
	bool isStart;
	RECT returnWindowRect;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//static extern int Getshared
};
