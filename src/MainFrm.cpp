
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Basin.h"

#include "MainFrm.h"
#include "util/debstr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() {
  debstr() << "CMainFrame ctor hello\n";
  // JG: SOmething is weird,  in debugger (and behaviour, it seems we only enter here at very late stage - after childview is set up.

	// TODO: add member initialization code here

  // JG: It seems we never enter here?
  // unsigned int myRandSeed = 314;
  // int badbad = 0; 
  //int funk = myRandSeed / badbad; 
  //srand(myRandSeed/badbad); // I want my player character, on start-up, to have some different stats..

}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	// Now we can start timer for m_wndView.
	const int ms100 = 100;
	m_wndView.SetTimer(timerID, ms100, NULL); // The purpose of this, is to get us to the _keyboard-event-loop_."
  debstr() << "one-shot timer that sets everything in motion.\n";

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU; // WS_MAXIMIZE | 

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

  cs.x = 5; cs.y = 5;
  cs.cx = 50 * 32; cs.cy = 31 * 32; 
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

