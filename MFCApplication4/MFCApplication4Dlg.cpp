
// MFCApplication4Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCApplication4.h"
#include "MFCApplication4Dlg.h"
#include "afxdialogex.h"
#include "DlgProcessList.h"
#include <gdiplus.h>//stdafx.h 또는 사용하려는 클래스의 헤더파일에 추가


#include <d3d9.h>
//#include <D3dx9tex.h>
//#include <d3dx9tex.h>



using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID 860515


// CMFCApplication4Dlg dialog
BOOL Capture(HWND hTargetWnd, LPCTSTR lpszFilePath);
BOOL Save(HBITMAP& hBitmap, CString strFilePath);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

/*
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
*/


CMFCApplication4Dlg::CMFCApplication4Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCApplication4Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	isStart = false;
	wndTarget = NULL;
}

void CMFCApplication4Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication4Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_START, &CMFCApplication4Dlg::OnBnClickedStart)
	ON_BN_CLICKED(ID_SEARCH, &CMFCApplication4Dlg::OnBnClickedSearch)
	ON_WM_TIMER()
	ON_WM_HOTKEY()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CMFCApplication4Dlg message handlers

BOOL CMFCApplication4Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	if(!::RegisterHotKey(GetSafeHwnd(), 5050 , NULL , VK_F4))		
		AfxMessageBox(_T("핫키 등록 실패!"));
	if(!::RegisterHotKey(GetSafeHwnd(), 5051 , NULL , VK_F3))		
		AfxMessageBox(_T("핫키 등록 실패!"));

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCApplication4Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication4Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL Capture(HWND hTargetWnd, LPCTSTR lpszFilePath)
{
    CRect rct;
    if(hTargetWnd)
        ::GetWindowRect(hTargetWnd, &rct);
    else
        return FALSE;

    HBITMAP hBitmap = NULL;
    HBITMAP hOldBitmap = NULL;
    BOOL bSuccess = FALSE;

    HDC hDC = ::GetDC(hTargetWnd);
    HDC hMemDC = ::CreateCompatibleDC(hDC);
    hBitmap = ::CreateCompatibleBitmap(hDC, rct.Width(), rct.Height());

    if(!hBitmap)
        return FALSE;

    hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    if(!::PrintWindow(hTargetWnd, hMemDC, 0))
        bSuccess = FALSE;
    else
        bSuccess = TRUE;

    //Save 함수 : http://blog.daum.net/odega/25
    if(bSuccess && !Save(hBitmap, lpszFilePath))
        bSuccess = FALSE;
    else
        bSuccess = TRUE;

    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    ::DeleteDC(hMemDC);
    ::ReleaseDC(hTargetWnd, hDC);

    return bSuccess;
}


BOOL Save(HBITMAP& hBitmap, CString strFilePath)
{
    if(!hBitmap)
        return FALSE;

    Gdiplus::Bitmap gBmp(hBitmap, NULL);
    CLSID clsid;
    GetEncoderClsid(_T("image/bmp"),&clsid);//HBITMAP을 bmp파일로 저장
    if(gBmp.Save(strFilePath, &clsid) == Gdiplus::Ok)
        return TRUE;//성공
    else
        return FALSE;//실패

    return TRUE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // 이미지 인코더의 개수
    UINT  size = 0;         // 이미지 인코더 배열의 바이트 크기

    ImageCodecInfo* pImageCodecInfo = NULL;
    GetImageEncodersSize(&num, &size);

    if(size == 0)
        return -1;  //실패

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));

    if(pImageCodecInfo == NULL)
        return -1;  //실패

    GetImageEncoders(num, size, pImageCodecInfo);//이미지 인코더 정보를 구함

    for(UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)//사용자가 요구한 그래픽 형태와 일치하면
        {
            *pClsid = pImageCodecInfo[j].Clsid;//코덱아이디값 설정
            free(pImageCodecInfo);
            return j;  //성공
        }
    }

    free(pImageCodecInfo);//해제
    return -1;  //실패
}


void CMFCApplication4Dlg::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here

	if(wndTarget == NULL)
	{
		AfxMessageBox(_T("프로그램을 지정해 주세요!"));
		return;
	}
	

	//int screenX = (int)GetSystemMetrics(SM_CXSCREEN);
	int screenY = (int)GetSystemMetrics(SM_CYSCREEN);

	RECT windowPos;
	if(!isStart)
	{
		GetDlgItem(ID_START)->SetWindowTextW(_T("중지"));
		// 윈도우 이동
		//wndTarget->ShowWindow(SW_RESTORE);
		//wndTarget->GetWindowRect(&returnWindowRect); 
		//wndTarget->SetWindowPos(NULL, 0, screenY, 0, 0, SWP_NOSIZE);
		// 스크린샷 시작
		SetTimer(TIMER_ID, 3000, NULL);
		isStart = true;
	}
	else
	{
		// 스크린샷 중지
		KillTimer(TIMER_ID);
		// 윈도우 이동
		//wndTarget->ShowWindow(SW_RESTORE);
		//wndTarget->SetWindowPos(NULL, returnWindowRect.left, returnWindowRect.top, 0, 0, SWP_NOSIZE);
		GetDlgItem(ID_START)->SetWindowTextW(_T("시작"));
		isStart = false;
	}
}


void CMFCApplication4Dlg::OnBnClickedSearch()
{
	// TODO: Add your control notification handler code here
	CDlgProcessList dlg;
	
	if (dlg.DoModal () == IDOK) {
		sProcessInfo _selProcess = *dlg._selPi;
		//wndTarget = CWnd::FromHandle (_selProcess.hWnd);
        //
	}
}


void CMFCApplication4Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CDialogEx::OnTimer(nIDEvent);

	CTime time = CTime::GetCurrentTime();
	switch(nIDEvent)
    {
        case TIMER_ID:
			TCHAR fileName[256];
			wsprintf(fileName, _T("%02d%02d.bmp"),time.GetMinute(), time.GetSecond());
			Capture(wndTarget, fileName);
            break;
    }
}


HBITMAP CopySurfaceToBitmap(LPDIRECT3DSURFACE9 pD3DSurface, BYTE *pData, BITMAPINFO *pHeader)
{
    HDC         hScrDC, hMemDC;         // surface DC and memory DC

    HBITMAP     hBitmap, hOldBitmap;    // handles to deice-dependent bitmaps

    int         nX, nY;		        // top-left of rectangle to grab

    int         nWidth, nHeight;        // DIB width and height

    

    // create a DC for the screen and create

    // a memory DC compatible to screen DC

    if (pD3DSurface == NULL)
	return NULL;

    pD3DSurface->GetDC(&hScrDC);
    hMemDC = CreateCompatibleDC(hScrDC);

    D3DSURFACE_DESC desc;
    pD3DSurface->GetDesc( &desc );

    // get points of rectangle to grab

    nX  = 0;
    nY  = 0;
    nWidth  = desc.Width;
    nHeight = desc.Height;

    // create a bitmap compatible with the surface DC

    hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

    // select new bitmap into memory DC

    hOldBitmap = (HBITMAP) SelectObject(hMemDC, hBitmap);

    // bitblt surface DC to memory DC

    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

    // select old bitmap back into memory DC and get handle to

    // bitmap of the screen   

    hBitmap = (HBITMAP)  SelectObject(hMemDC, hOldBitmap);

    // Copy the bitmap data into the provided BYTE buffer

    GetDIBits(hScrDC, hBitmap, 0, nHeight, pData, pHeader, DIB_RGB_COLORS);

    // clean up

    pD3DSurface->ReleaseDC(hScrDC);
    DeleteDC(hMemDC);

    // return handle to the bitmap

    return hBitmap;
}


BOOL SaveToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;

	int iBits;

	WORD wBitCount;

	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0;

	BITMAP Bitmap;

	BITMAPFILEHEADER bmfHdr;

	BITMAPINFOHEADER bi;

	LPBITMAPINFOHEADER lpbi;

	HANDLE fh, hDib, hPal,hOldPal=NULL;

	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{ 
		hDC = GetDC(NULL);
		hOldPal = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE; 

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}


IDirect3D9*			g_pD3D=NULL;
IDirect3DDevice9*	g_pd3dDevice=NULL;
IDirect3DSurface9*	g_pSurface=NULL;


HRESULT	InitD3D(HWND hWnd)
{
	D3DDISPLAYMODE	ddm;
	D3DPRESENT_PARAMETERS	d3dpp;

	if((g_pD3D=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
	{
		return E_FAIL;
	}

	if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm)))
	{
		return E_FAIL;
	}

	ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.Windowed=TRUE;
	d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	d3dpp.BackBufferFormat=ddm.Format;
    d3dpp.BackBufferWidth = ddm.Width;
    d3dpp.BackBufferHeight = ddm.Height;
	d3dpp.MultiSampleType=D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow=hWnd;
	d3dpp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	d3dpp.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;

	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING ,&d3dpp,&g_pd3dDevice)))
	{
		return E_FAIL;
	}

	if(FAILED(g_pd3dDevice->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &g_pSurface, NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HDC		hBackDC=NULL;
LPVOID	pBits=NULL;
HBITMAP	hBackBitmap=NULL;
HBITMAP	hOldBitmap=NULL;

void captureDX(HWND hWnd)
{
    CoInitialize(NULL);

    HWND hwnd = hWnd;

    LPDIRECT3D9 d3d9;
    D3DDISPLAYMODE ddm;

    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    d3d9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&ddm);

	///////////////////////////////////

	g_pd3dDevice->GetFrontBufferData(0, g_pSurface);			
	//D3DXSaveSurfaceToFile(szFileName, D3DXIFF_BMP, g_pSurface,NULL,NULL);		//Save to File

	D3DLOCKED_RECT	lockedRect;
	if(FAILED(g_pSurface->LockRect(&lockedRect,NULL,D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY)))
	{
	}
	else
	{	
		CRect rct;
		if(hWnd)
			::GetWindowRect(hWnd, &rct);
		else
			return ;

		BITMAPINFO dib_define;
		ZeroMemory(&dib_define,sizeof(BITMAPINFO));
		dib_define.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib_define.bmiHeader.biWidth = rct.right - rct.left;
		dib_define.bmiHeader.biHeight = rct.bottom - rct.top;
		dib_define.bmiHeader.biPlanes = 1;
		dib_define.bmiHeader.biBitCount = 24;
		dib_define.bmiHeader.biCompression = BI_RGB;
		dib_define.bmiHeader.biSizeImage = ((((rct.right - rct.left) * 24 + 31) & ~31) >> 3) * (rct.bottom - rct.top);
		dib_define.bmiHeader.biXPelsPerMeter = 0;
		dib_define.bmiHeader.biYPelsPerMeter = 0;
		dib_define.bmiHeader.biClrImportant = 0;
		dib_define.bmiHeader.biClrUsed = 0;

		//BITMAPINFO	bmpInfo;
		//ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		//bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		//bmpInfo.bmiHeader.biBitCount=BITSPERPIXEL;
		//bmpInfo.bmiHeader.biCompression = BI_RGB;
		//bmpInfo.bmiHeader.biWidth=GetSystemMetrics(SM_CXSCREEN);
		//bmpInfo.bmiHeader.biHeight=GetSystemMetrics(SM_CYSCREEN);
		//bmpInfo.bmiHeader.biPlanes=1;
		//bmpInfo.bmiHeader.biSizeImage=abs(bmpInfo.bmiHeader.biHeight)*bmpInfo.bmiHeader.biWidth*bmpInfo.bmiHeader.biBitCount/8;

		HDC	hdc=GetDC(hWnd);
		hBackDC=CreateCompatibleDC(hdc);
		hBackBitmap=CreateDIBSection(hdc,&dib_define,DIB_RGB_COLORS,&pBits,NULL,0);
		if(hBackBitmap==NULL)
		{
		}
		ReleaseDC(hWnd,hdc);

		for(int i=0;i<rct.bottom;i++)
		{
			memcpy((BYTE*)pBits+(rct.bottom-i-1)*rct.right*32/8,(BYTE*)lockedRect.pBits+i*lockedRect.Pitch,rct.right*32/8);//g_d3dpp.BackBufferHeight*g_d3dpp.BackBufferWidth*4);				
		}
		HBITMAP hBitmap = CopySurfaceToBitmap(g_pSurface, (LPBYTE)pBits, &dib_define);


		CTime time = CTime::GetCurrentTime();
		TCHAR fileName[256];
		wsprintf(fileName, _T("%02d%02d.bmp"),time.GetMinute(), time.GetSecond());
		Save(hBitmap, fileName);
		//SaveToFile(hBitmap, fileName);
	}

	g_pSurface->UnlockRect();
	

	return;


    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp,sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = ddm.Width;
    d3dpp.BackBufferHeight = ddm.Height;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.hDeviceWindow = hwnd;
    d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;


    LPDIRECT3DDEVICE9 dev;
    d3d9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING ,&d3dpp, &dev);

    /* Deprecation
    IDirect3DSurface9* surface = NULL;
    dev->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &surface, NULL);
    dev->GetFrontBufferData(0, surface);
    D3DXSaveSurfaceToFile(L"D:\\test.bmp", D3DXIFF_BMP, surface, NULL, NULL);
    */

    // Transfer the data of a back buffer to the system memory
    IDirect3DSurface9* surface = NULL;
    dev->CreateOffscreenPlainSurface(ddm.Width, ddm.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL);
    LPDIRECT3DSURFACE9 back = NULL;
    dev->SetRenderTarget(0, surface);
    dev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back);
    dev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,127), 1.0, 0 );
    dev->BeginScene();
    dev->StretchRect(surface, NULL, back, NULL, D3DTEXF_NONE);
    dev->EndScene();
    dev->Present(0,0,0,0);
	
    //D3DXSaveSurfaceToFile(L"D:\\test.bmp", D3DXIFF_BMP, back, NULL, NULL);

	/*
	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);
	D3DLOCKED_RECT d3dlr;
	BYTE  *pSurfaceBuffer;
	BYTE  *pData = NULL;

	surface->LockRect(&d3dlr, 0, D3DLOCK_DONOTWAIT);

        //avoiding  vertical flip

	pSurfaceBuffer = (BYTE *) d3dlr.pBits + d3dlr.Pitch*(surfaceDesc.Height - 1);
        
        //video sample pitch, forcing ARGB 32bits

	int m_lVidPitch  = (surfaceDesc.Width * 4 + 4) & ~(4);

	for (int i=0;i<(int)surfaceDesc.Height;i++)
	{
	    BYTE *pDataOld = pData;
            BYTE *pSurfaceBufferOld = pSurfaceBuffer; 

 	    for (int j=0;j<(int)surfaceDesc.Width;j++)
	    {
			
		pData[0] = pSurfaceBuffer[0];
		pData[1] = pSurfaceBuffer[1];
		pData[2] = pSurfaceBuffer[2];
		pData[3] = pSurfaceBuffer[3];
			
		pData+=4; pSurfaceBuffer+=4;
	    }
            //next video sample row

            pData = pDataOld + m_lVidPitch;
            //previous surface row

            pSurfaceBuffer = pSurfaceBufferOld - d3dlr.Pitch;
	}

	surface->UnlockRect();


	 CRect rct;
	if(hWnd)
		::GetWindowRect(hWnd, &rct);
	else
		return ;
	
	BITMAPINFO dib_define;
	dib_define.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	dib_define.bmiHeader.biWidth = rct.right - rct.left;
	dib_define.bmiHeader.biHeight = rct.bottom - rct.top;
	dib_define.bmiHeader.biPlanes = 1;
	dib_define.bmiHeader.biBitCount = 24;
	dib_define.bmiHeader.biCompression = BI_RGB;
	dib_define.bmiHeader.biSizeImage = ((((rct.right - rct.left) * 24 + 31) & ~31) >> 3) * (rct.bottom - rct.top);
	dib_define.bmiHeader.biXPelsPerMeter = 0;
	dib_define.bmiHeader.biYPelsPerMeter = 0;
	dib_define.bmiHeader.biClrImportant = 0;
	dib_define.bmiHeader.biClrUsed = 0;

	 HBITMAP hBitmap = CopySurfaceToBitmap(surface, pData, &dib_define);

	 CTime time = CTime::GetCurrentTime();
	TCHAR fileName[256];
	wsprintf(fileName, _T("%02d%02d.bmp"),time.GetMinute(), time.GetSecond());
	 SaveToFile(hBitmap, fileName);
	 */

	/*
	 CRect rct;
	if(hWnd)
		::GetWindowRect(hWnd, &rct);
	else
		return ;
	HDC hDC;// = ::GetDC(hTargetWnd);
	back->GetDC(&hDC);
    HDC hMemDC = ::CreateCompatibleDC(hDC);
    HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, rct.Width(), rct.Height());

    if(!hBitmap)
        return ;

    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);


	::BitBlt(hMemDC,0,0,240,320,hDC,0,0,SRCCOPY);
	CTime time = CTime::GetCurrentTime();
	TCHAR fileName[256];
	wsprintf(fileName, _T("%02d%02d.bmp"),time.GetMinute(), time.GetSecond());
	Save(hBitmap, fileName);

    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    ::DeleteDC(hMemDC);
    ::ReleaseDC(hWnd, hDC);
	*/

    if(back) back->Release();

    dev->Release();
    d3d9->Release();

    CoUninitialize();
}

void CMFCApplication4Dlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default
	if((int)nHotKeyId == 5050)
	{
		POINT pt;
		::GetCursorPos(&pt);
		wndTarget = ::WindowFromPoint(pt);

		 CRect rct;
		if(wndTarget)
			::GetWindowRect(wndTarget, &rct);
		else
			return ;

	
		//captureDX(wndTarget);
		//return;

		HRESULT hr = S_OK;
	

		// Register the thumbnail
		hr = DwmRegisterThumbnail(this->GetSafeHwnd(), wndTarget, &thumbnail);
		if (SUCCEEDED(hr))
		{
			this->SetWindowPos(NULL, 0, 0, rct.right- rct.left, rct.bottom - rct.top, SWP_NOMOVE);
			// Specify the destination rectangle size
			RECT dest = {0,};
			
			dest.left = 0;
			dest.top = 0;
			dest.right = rct.right- rct.left;
			dest.bottom = rct.bottom - rct.top;
			

			// Set the thumbnail properties for use
			DWM_THUMBNAIL_PROPERTIES dskThumbProps;
			//dskThumbProps.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_VISIBLE | DWM_TNP_OPACITY | DWM_TNP_RECTDESTINATION;
			dskThumbProps.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_SOURCECLIENTAREAONLY;
			dskThumbProps.fSourceClientAreaOnly = FALSE; 
			dskThumbProps.fVisible = TRUE;
			dskThumbProps.opacity = (255 * 100)/100;
			dskThumbProps.rcDestination = dest;

			// Display the thumbnail
			hr = DwmUpdateThumbnailProperties(thumbnail,&dskThumbProps);

			if (SUCCEEDED(hr))
			{
				InitD3D(this->GetSafeHwnd());
				captureDX(this->GetSafeHwnd());
				// ...
			}
		}
		
	}
	else if((int)nHotKeyId == 5051)
	{

		
		CTime time = CTime::GetCurrentTime();
		TCHAR fileName[256];
		wsprintf(fileName, _T("%02d%02d.bmp"),time.GetMinute(), time.GetSecond());
	
		Capture(this->GetSafeHwnd(), fileName);
	}
	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}


int CMFCApplication4Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
