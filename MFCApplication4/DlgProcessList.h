#pragma once
#include <vector>
#include "afxcmn.h"

// CDlgProcessList dialog

using namespace std;

struct sProcessInfo 
{
	HWND hWnd;
	DWORD processID;
	DWORD threadID;

	sProcessInfo () :
		hWnd(0), processID(0), threadID(0) {}

	sProcessInfo (HWND hWnd_, DWORD processID_, DWORD threadID_) :
		hWnd(hWnd_), processID(processID_), threadID(threadID_) {}
};


class CDlgProcessList : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessList)

public:
	CDlgProcessList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProcessList();

// Dialog Data
	enum { IDD = IDD_DLG_PROCESS };

	sProcessInfo *_selPi;

private:
	void UpdateProcessListView ();
	void UpdateListProcess ();
	int  GetProcessList ();

private:
	HWND _selProcess;

	CImageList _processIcon;
	
	vector<sProcessInfo> _processList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);


	DECLARE_MESSAGE_MAP()
public:
	CListCtrl _listProcess;
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedOk();
};
