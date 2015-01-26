// DlgProcessList.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication4.h"
#include "DlgProcessList.h"
#include "afxdialogex.h"


HICON GetWindowsIcon (HWND hWnd);
// CDlgProcessList dialog

IMPLEMENT_DYNAMIC(CDlgProcessList, CDialog)

CDlgProcessList::CDlgProcessList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessList::IDD, pParent)
{
	_selProcess = NULL;
	_selPi = NULL;
}

CDlgProcessList::~CDlgProcessList()
{
}

void CDlgProcessList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, _listProcess);
}


BEGIN_MESSAGE_MAP(CDlgProcessList, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDlgProcessList::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDOK, &CDlgProcessList::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CDlgProcessList::OnInitDialog()
{
	CDialog::OnInitDialog();

	_processIcon.Create (16, 16, ILC_COLOR24|ILC_MASK, 10,	10);

	_listProcess.SetExtendedStyle(_listProcess.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER | LVS_EX_SINGLEROW);
	_listProcess.SetImageList(&_processIcon, LVSIL_SMALL);
	_listProcess.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	
	_listProcess.InsertColumn(0, _T("프로세서 이름"),	LVCFMT_LEFT, 330);
	_listProcess.InsertColumn(1, _T("상태"),			LVCFMT_LEFT, 75);

	// 응용 프로그램 목록얻기
	GetProcessList ();
	UpdateProcessListView ();

	SetTimer (1000, 100, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}



int CDlgProcessList::GetProcessList ()
{
	_processList.clear ();
	
	CWnd *mainWnd = AfxGetMainWnd ();

	// 처음 최상위 윈도우를 목록에서 얻어온다.
	int count = 0;
	for (CWnd *pWnd = mainWnd->GetWindow(GW_HWNDFIRST); pWnd; pWnd = pWnd->GetWindow(GW_HWNDNEXT)) {
		if (!pWnd->GetOwner() && pWnd->IsWindowVisible() && pWnd->GetWindowTextLength()){
			// 윈도우의 캡션을 얻음		
			CString strCaption;
			pWnd->GetWindowText(strCaption); 

			if (strCaption == "InactiveWinMacro") continue;
			if (strCaption == "비활성 윈도우 매크로") continue;
			if (strCaption == "Program Manager") continue;

			DWORD processID = 0;
			DWORD threadID  = ::GetWindowThreadProcessId (pWnd->m_hWnd, &processID);
			
			_processList.push_back (sProcessInfo(pWnd->m_hWnd, processID, threadID));
			count ++;
		}
	}

	return count;
}

void CDlgProcessList::UpdateProcessListView ()
{
	// 프로세서와 연결된 윈도우 아이콘 이미지를 모두 지운다.
	for (int i=_processIcon.GetImageCount()-1; i>=0; i--) {
		_processIcon.Remove(i);
	}

	// 현재 선택된 항목 저장해 둔다.
	int sel = _listProcess.GetNextItem (-1, LVNI_SELECTED);

	// 리스트 컨트롤에 등록된 모든 프로세서 목록도 지운다.
	_listProcess.DeleteAllItems (); 

	// 검색된 프로세서 목록으로 리스트 컨트롤을 새로 채운다.
	for (int i=0; i<(int)_processList.size(); i++) {
		sProcessInfo &pi = _processList[i];
		CWnd *pWnd = CWnd::FromHandle (pi.hWnd);

		CString strCaption;
		pWnd->GetWindowText(strCaption); // 윈도우의 캡션을 얻음

		HICON icon = GetWindowsIcon (pi.hWnd);
		_processIcon.Add (icon);

		_listProcess.InsertItem  (i, strCaption);
		_listProcess.SetItemText (i, 1, _T("실행 중"));
		_listProcess.SetItemData (i, pi.threadID);
		_listProcess.SetItem     (i, 0, LVIF_IMAGE, NULL, i, 0, 0, 0);
	}

	// 윈도우가 하나 이상 있으면 제일 처음 윈도우를 선택한다.
	if (sel >= 0 && _listProcess.GetItemCount () > sel) {
		_listProcess.SetItemState (sel, LVIS_SELECTED, LVIS_SELECTED);
		_listProcess.EnsureVisible (sel-1, FALSE);
	}
}

void CDlgProcessList::UpdateListProcess ()
{
	for (int i=0; i<(int)_processList.size(); i++) {
		sProcessInfo &pi = _processList[i];
		
		DWORD tid = ::GetWindowThreadProcessId (pi.hWnd, NULL);

		_listProcess.SetItemText (i, 1, tid ? _T("실행 중"): _T("종료"));
	}
}

void CDlgProcessList::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		// 프로세서가 실행중인지 종료되었는지의 상태를 업데이트 한다.
		UpdateListProcess ();
	}

	CDialog::OnTimer(nIDEvent);
}


// CDlgProcessList message handlers


void CDlgProcessList::OnBnClickedButtonRefresh()
{
	// 응용 프로그램 목록얻기
	GetProcessList ();
	UpdateProcessListView ();

	_selProcess = NULL;
}



HICON GetWindowsIcon (HWND hWnd)
{
	HICON hIcon = NULL;

	if (SendMessageTimeout (hWnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 1000, (DWORD_PTR *)&hIcon)) {
		if (hIcon) return hIcon;
	}
	if (SendMessageTimeout(hWnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 1000, (DWORD_PTR *)&hIcon)) {
		if (hIcon) return hIcon;
	}

	hIcon = (HICON)GetClassLong(hWnd, GCL_HICON);
	if (hIcon) return hIcon;

	hIcon = (HICON)GetClassLong(hWnd, GCL_HICONSM);
	if (hIcon) return hIcon;

	hIcon = LoadIcon(NULL, IDI_APPLICATION);
	return hIcon;
}

void CDlgProcessList::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int sel = _listProcess.GetNextItem (-1, LVNI_SELECTED);
	if (0 <= sel && sel < (int)_processList.size()) {
		sProcessInfo &pi = _processList[sel];
		
		// 선택된 프로세서가 현재 실행 중인지 체크한다.
		DWORD tid = ::GetWindowThreadProcessId (pi.hWnd, NULL);
		if (tid) {
			// 선택된 프로세서 정보에 대한 포인터를 복사해 둔다.
			// 대화상자를 호출한 윈도우에 리턴 값으로 사용됨
			_selPi = &pi;

			OnOK();
		}
		else AfxMessageBox (_T("현재 실행 중이지 않은 프로세서는 선택할 수 없습니다."));
	}
	else AfxMessageBox (_T("프로세서를 선택하지 않았습니다."));


	CDialog::OnOK();
}
