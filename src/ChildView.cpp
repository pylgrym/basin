
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Basin.h"
#include "ChildView.h"

#include "util/debstr.h"

#include "./theUI.h"

#include "numutil/myrnd.h"

#include "Bag.h"
#include "Term.h"

#include "MobQueue.h"

#include "Dungeons.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView() {
  singletonWnd = this;

  TCHAR dirBufU[256 + 1];
  // what does sizeof actually do? Answer: size in BYTES.
  ::GetCurrentDirectory(sizeof dirBufU/sizeof(TCHAR), dirBufU);

  CT2A dirBufA(dirBufU, CP_ACP);
  // CA2T uc(s.c_str(), CP_ACP); //  CP_UTF8);

  debstr() << "CChildView ctor, cur dir:" << dirBufA << std::endl; 
  // Conclusion: current dir is: 
  // "D:\moria\Basin\src\"

  PlayerMob* player = PlayerMob::createPlayer();
  // Dungeon* L1 = Dungeons::get(1);
  Dungeons::setCurLevel(player->dungLevel);

  // This is a little bit bad, because it even
  // atempts to trigger a redraw, at a time where we don't have any HWND yet..
  PlayerMob::ply->passTime(); // Hack to make player-LIGHT init correctly; could be handled many other ways.

  //FIXME: (DARKNESS) img-tiles should be BLACK!
}



CChildView::~CChildView() {
  singletonWnd = NULL;
}

void TheUI::BeepWarn() {
  const int ms500 = 500;
  const int freq400 = 400;
  Beep(freq400, ms500);
  if (CChildView::singletonWnd != NULL) {
    CChildView::singletonWnd->FlashWindow(true);
  }
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
  // ON_WM_CHAR()
END_MESSAGE_MAP()



bool TheUI::shiftKey() {
  SHORT state = GetAsyncKeyState(VK_SHIFT);
  return (state < 0); // If most significant bit is set, it's negative.
}

bool TheUI::ctrlKey() { return (GetAsyncKeyState(VK_CONTROL) < 0); }


unsigned int TheUI::getNextKey() {  // UINT
	// GetMessage loop example. // http://www.cplusplus.com/forum/beginner/38860/	
  debstr() << "WAITING for user keyboard input (getNextKey)\n";
	MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
		// if (msg.message == WM_KEYDOWN) {
		// 	UINT keyCode = msg.wParam;
		// 	return keyCode;
		// }

		TranslateMessage(&msg); // 
		DispatchMessage(&msg);

    if (msg.message == WM_KEYDOWN) { // WM_CHAR) {
      unsigned int charCode = msg.wParam;
      debstr() << "got key:" << (char) charCode << " " << (void*) charCode << "\n";
      return charCode;
    }

	} // If we get out of here (return value not == 0), it means we got WM_QUIT. (or < 0 on error.)

	return VK_CANCEL; // bad, the user is closing app, WM_QUIT!
}



 int TheUI::microSleepForRedraw(int maxCount) {  
	// GetMessage loop example. // http://www.cplusplus.com/forum/beginner/38860/	
  debstr() << "microSleep, allowing redraw.\n";
  int count = 0;
	MSG msg;
  // GetMessage(&msg, NULL, 0, 0) > 0 
  while ( PeekMessage(&msg,NULL,0,0, PM_REMOVE) > 0 && count < maxCount) {
    ++count;
    const int sleepMS = 10; // 4;
    Sleep(sleepMS);
		TranslateMessage(&msg);  
		DispatchMessage(&msg);
	} // If we get out of here (return value not == 0), it means we got WM_QUIT. (or < 0 on error.)

  return 0; // VK_CANCEL; // bad, the user is closing app, WM_QUIT!
}





// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;

  COLORREF darkScreenBackground = RGB(10, 15, 20);
  static CBrush background(darkScreenBackground); // (static: Important - it must live on, for color to stay..)
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), 
    background, //reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), 
    NULL);

	return TRUE;
}


void CChildView::OnTimer(UINT nIDEvent) { // Used to start app loop.
	debstr() << "ontimer:" << nIDEvent << "\n";
	KillTimer(timerID);
	// do stuff..
  debstr() << "Starting queue-process..\n";
  for (bool isRunning=true; isRunning; ) {
    isRunning = CL->mobs.dispatchFirst();
    // debstr() << "isRunning?" << isRunning << "\n";
    if (PlayerMob::ply->isDead()) {
      logstr log; log << "!You have died!";
      isRunning = false;
    }
  }
  debstr() << "ended queue-process.\n";
}






void doFont(CFont& largeFont, CFont& smallFont, CPaintDC& dc) {
	LOGFONT logFont;
	CFont* pOldFont = dc.GetCurrentFont();
	pOldFont->GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;
	logFont.lfWidth = 0;

	// -17 and terminal, 'fits', but is too spaced out.
	// -28 and terminal, has proper widths, but breaks gyjgyj outliers.
	// -24 is a compromise.
	double fontScale = (18.0/20.0);
	logFont.lfHeight = (int) (-Tiles::TileWidth * fontScale); //-18; //-24; // 14*1.3f; //-50; //22;
	// const char* fontName = "Terminal"; //"STENCIL"; // "Terminal"
	const TCHAR* fontName = L"Rockwell Extra Bold"; //"STENCIL"; // "Terminal"

  // strncpy_s
	wcsncpy_s(logFont.lfFaceName, sizeof logFont.lfFaceName / sizeof (TCHAR), fontName, sizeof logFont.lfFaceName / sizeof (TCHAR));

	largeFont.CreateFontIndirect(&logFont);

	double smallScale = (7.0/20.0); // was: 9.0
	logFont.lfHeight = (int) (-Tiles::TileWidth * smallScale);
	logFont.lfWeight = FW_THIN;
	smallFont.CreateFontIndirect(&logFont);
}




CChildView* CChildView::singletonWnd = NULL;

void TheUI::invalidateWndJG(CRect* pRect, bool erase) { 
  // (invalidateWndJG: Actual connection to redraw/invalidate window.)
  if (CChildView::singletonWnd == NULL) { debstr() << "no sing wnd?\n";  return; }
  if (CChildView::singletonWnd->GetSafeHwnd() == NULL) { debstr() << "no hWnd yet\n";  return; }
  CChildView::singletonWnd->InvalidateRect(pRect, erase);
}



void TheUI::invalidateCell(CPoint w_tilepos) { 
  // (invalidateCell: same as invalidateCellXY, just convenient interface.)
  invalidateCellXY(w_tilepos.x, w_tilepos.y);  
}



void TheUI::invalidateCellXY(int w_tx, int w_ty) {
  // (invalidateCellXY: exposed interface func.)
  CPoint wp(w_tx,w_ty); // world

  VPoint vp;
  vp.p = Viewport::vp.w2v(wp); // wp + Viewport::vp.offset;
  invalidateVPCell(vp.p);
}


void TheUI::invalidateVPCell(CPoint vp) { 
  int px = vp.x * Tiles::TileWidth, py = vp.y * Tiles::TileHeight;
	CRect cellR(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));
  invalidateWndJG(&cellR, false);
}


CPoint Viewport::w2v(CPoint w) { return w - offset; }
CPoint Viewport::v2w(CPoint v) { return v + offset; }


void CChildView::OnPaint() {
	CPaintDC dc(this); // device context for painting

  CFont largeFont, smallFont;
  doFont(largeFont,smallFont, dc);
	dc.SelectObject(largeFont);

  dc.SetBkMode(TRANSPARENT);
  CBrush txtBk(RGB(0, 0, 20));

  VPoint vp;
  for (vp.p.x = 0; vp.p.x < Term::Width; ++vp.p.x) { // Viewport::Width
    for (vp.p.y = 0; vp.p.y < Term::Height; ++vp.p.y) { // Viewport::Height
      CPoint wp = Viewport::vp.v2w(vp.p); // + Viewport::vp.offset;

      // map will return 'nil items' when you ask outside range, because we need to clear/draw outside fields too.
      CellColumn& column = CL->map[wp.x]; // VIEWPORT STUFF. // x + Viewport::vp.offset.x
      Cell& cell = column[wp.y];           // VIEWPORT STUFF. // y + Viewport::vp.offset.y];
      TCell& tcell = Term::term[vp.p];

      tiles.drawTile(vp.p.x, vp.p.y, cell.envir.typeS(), dc, false, 255); // FLOOR



      if (!cell.item.empty()) { 
        tiles.drawTile(vp.p.x, vp.p.y, cell.item.typeS(), dc, true,255); // false);  // THINGS

        int px = vp.p.x * Tiles::TileWidth, py = vp.p.y * Tiles::TileHeight;
        CRect cellR(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));
        CString s; s.Format(L"<%d>", cell.item.o->ilevel);

        dc.SelectObject(smallFont);
        // const int fontFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
        const int fontFlags = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
        dc.SetTextColor(RGB(0,255,0)); // RGB(0, 0, 255)); // RED.
        dc.DrawText(s, &cellR, fontFlags);
      }



      if (!cell.creature.empty()) { 
        tiles.drawTileA(vp.p.x, vp.p.y, cell.creature.typeS(), dc, true,255); // false); MOBS

        // Draw stats/HP:
        Mob* mob = cell.creature.m;
		    int px = vp.p.x * Tiles::TileWidth, py = vp.p.y * Tiles::TileHeight;
		    CRect cellR( CPoint(px,py), CSize(Tiles::TileWidth,Tiles::TileHeight));
        CString s; s.Format(L"%d ", mob->stats.hp);

	      dc.SelectObject(smallFont);
        // const int fontFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
        const int fontFlags = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
        dc.SetTextColor(mob->color); // RGB(0, 0, 255)); // RED.
		    dc.DrawText(s, &cellR,  fontFlags);
        dc.SetTextColor(RGB(0,0,0)); // BLACK.
		    cellR.OffsetRect(2, 2); dc.DrawText(s, &cellR, fontFlags);
        dc.SetTextColor(RGB(255, 255, 255)); // WHITE.
		    cellR.OffsetRect(-1, -1); dc.DrawText(s, &cellR, fontFlags);

        if (false) {
          // Draw level upper right:
          const int upperRightFlags = DT_RIGHT | DT_TOP | DT_SINGLELINE;
          dc.SetTextColor(RGB(0, 255, 0)); // RGB(0, 0, 255)); // RED.
          s.Format(L"%dL", mob->stats.level());
          dc.DrawText(s, &cellR, upperRightFlags);
        }


        if (false) {
          // Draw str upper left:
          const int upperLeftFlags = DT_LEFT | DT_TOP | DT_SINGLELINE;
          dc.SetTextColor(RGB(255, 0, 0)); //strength is red.
          s.Format(L"%d", mob->stats.Str.v);
          dc.DrawText(s, &cellR, upperLeftFlags);
        }

        if (false) {
          // Draw dex lower left:
          const int lowerLeftFlags = DT_LEFT | DT_BOTTOM | DT_SINGLELINE;
          dc.SetTextColor(RGB(0,0,255)); // dex is blue.
          s.Format(L"%d", mob->stats.Dex.v);
		      dc.DrawText(s, &cellR,  lowerLeftFlags);
        }
      }

      if (!cell.hasOverlay()) { 
        tiles.drawTileB(vp.p.x, vp.p.y, cell.overlay, dc, true,255); // false);  // THINGS
      }

      if (!tcell.charEmpty()) { 
	      dc.SelectObject(largeFont);
        const COLORREF txtColor = RGB(255, 255, 255);
        dc.SetTextColor(txtColor);  
		    int px = vp.p.x * Tiles::TileWidth, py = vp.p.y * Tiles::TileHeight;
		    CRect cellR( CPoint(px,py), CSize(Tiles::TileWidth,Tiles::TileHeight));
        dc.FillRect(&cellR, &txtBk);
        CString s; s.Format(L"%c", tcell.c);
		    dc.DrawText(s, &cellR,  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      } else { // Only draw light-shading when not text-cell:

        if (!cell.light()) {
          // Base darkening on tile-distance to player:
          int dist = PlayerMob::distPlyLight(CPoint(wp.x, wp.y));
          if (dist < 0) { dist = 0;  }
          int blend = (int) (255.0 - (255.0 / (dist+1)));

          CPoint blendTile(29,20); 
          tiles.drawTileB(vp.p.x, vp.p.y, blendTile, dc, true, blend); 
        }
      }

    }
  }

	
}







