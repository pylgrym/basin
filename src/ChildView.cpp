
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "Basin.h"
#include "ChildView.h"

#include "util/debstr.h"

#include "./theUI.h"

#include "numutil/myrnd.h"

#include "Bag.h"

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

  // Populate world:

  // FIXME: new + 'firstmove'+queue should become a single function.
  /* Figure out dependency order of map, creatures, queues, etc.*/


  Mob* player = new PlayerMob;
  Map::map.moveMob(*player, player->pos);
  MobQueue::mobs.queueMob(player,0);

  const int mobCount = 10; // 100;
  for (int i=0; i<mobCount; ++i) {
    debstr() << "i:" << i << "\n";

    Mob* monster = new MonsterMob;
    Map::map.moveMob(*monster, monster->pos);
    MobQueue::mobs.queueMob(monster,1);
  }

  /*
  const int itemCount = 10; // 100;
  for (int i = 0; i < itemCount; ++i) {
    debstr() << "i:" << i << "\n";

    CPoint pos;
    pos.x = rnd(1, Map::Width-1); 
    pos.y = rnd(1, Map::Height-1);
    if (Map::map[pos].blocked()) { continue; } // Don't add item on blocked floor.

    Obj* obj = new Obj(OB_Gold); // (L"Lamp");

    // color = RGB(rand()%255,rand()%255,rand()%255);
    Map::map.addObj(*obj, pos);
  }
  */

  std::stringstream ignore;
  Bag::bag.add(new Obj(OB_Hat),ignore);
  Bag::bag.add(new Obj(OB_Sword),ignore);
  // Bag::bag.add(new Obj(OB_Gold),ignore);
  Bag::bag.add(new Obj(OB_Lamp),ignore);
  Bag::bag.add(new Obj(OB_Hat),ignore);

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



 int TheUI::microSleepForRedraw() {  // UINT
	// GetMessage loop example. // http://www.cplusplus.com/forum/beginner/38860/	
  debstr() << "microSleep, allowing redraw.\n";
  int count = 0;
	MSG msg;
  // GetMessage(&msg, NULL, 0, 0) > 0 
  while ( PeekMessage(&msg,NULL,0,0, PM_REMOVE) > 0 && count < 4) {
    ++count;
    Sleep(2);
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
    isRunning = MobQueue::mobs.dispatchFirst();
    // debstr() << "isRunning?" << isRunning << "\n";
  }
  debstr() << "ended queue-process.\n";
}




CChildView* CChildView::singletonWnd = NULL;

void TheUI::invalidateWndJG(CRect* pRect, bool erase) { 
  // (invalidateWndJG: Actual connection to redraw/invalidate window.)
  if (CChildView::singletonWnd == NULL) { debstr() << "no sing wnd?\n";  return; }
  CChildView::singletonWnd->InvalidateRect(pRect, erase); 
}


void TheUI::invalidateCellXY(int tx, int ty) {
  // (invalidateCellXY: exposed interface func.)
	int px = tx * Tiles::TileWidth, py = ty * Tiles::TileHeight;
	CRect cellR(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));
  invalidateWndJG(&cellR, false);
}

void TheUI::invalidateCell(CPoint tilepos) { 
  // (invalidateCell: same as invalidateCellXY, just convenient interface.)
  invalidateCellXY(tilepos.x, tilepos.y);  
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






void CChildView::OnPaint() {
	CPaintDC dc(this); // device context for painting

  CFont largeFont, smallFont;
  doFont(largeFont,smallFont, dc);
	dc.SelectObject(largeFont);

  dc.SetBkMode(TRANSPARENT);
  CBrush txtBk(RGB(0, 0, 20));

  for (int x = 0; x < Map::Width; ++x) {
    CellColumn& column = Map::map[x];
    for (int y = 0; y < Map::Height; ++y) {
      Cell& cell = column[y];

      tiles.drawTile(x, y, cell.envir.typeS(), dc, false, 255); // FLOOR

      if (!cell.item.empty()) { 
        tiles.drawTile(x, y, cell.item.typeS(), dc, true,255); // false);  // THINGS
      }

      if (!cell.creature.empty()) { 
        tiles.drawTile(x, y, cell.creature.typeS(), dc, true,255); // false); MOBS

        // Draw stats/HP:
        Mob* mob = cell.creature.m;
		    int px = mob->pos.x * Tiles::TileWidth, py = mob->pos.y * Tiles::TileHeight;
		    CRect cellR( CPoint(px,py), CSize(Tiles::TileWidth,Tiles::TileHeight));
        CString s; s.Format(L"%d", mob->stats.hp);

	      dc.SelectObject(smallFont);
        // const int fontFlags = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
        const int fontFlags = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
        dc.SetTextColor(mob->color); // RGB(0, 0, 255)); // RED.
		    dc.DrawText(s, &cellR,  fontFlags);
        dc.SetTextColor(RGB(0,0,0)); // BLACK.
		    cellR.OffsetRect(2, 2); dc.DrawText(s, &cellR, fontFlags);
        dc.SetTextColor(RGB(255, 255, 255)); // WHITE.
		    cellR.OffsetRect(-1, -1); dc.DrawText(s, &cellR, fontFlags);

        // Draw level upper right:
        const int upperRightFlags = DT_RIGHT | DT_TOP | DT_SINGLELINE;
        dc.SetTextColor(RGB(0,255,0)); // RGB(0, 0, 255)); // RED.
        s.Format(L"%dL", mob->stats.level());
		    dc.DrawText(s, &cellR,  upperRightFlags);

        // Draw str upper left:
        const int upperLeftFlags = DT_LEFT | DT_TOP | DT_SINGLELINE;
        dc.SetTextColor(RGB(255,0,0)); //strength is red.
        s.Format(L"%d", mob->stats.s["str"].v);
		    dc.DrawText(s, &cellR,  upperLeftFlags);

        // Draw dex lower left:
        const int lowerLeftFlags = DT_LEFT | DT_BOTTOM | DT_SINGLELINE;
        dc.SetTextColor(RGB(0,0,255)); // dex is blue.
        s.Format(L"%d", mob->stats.s["dex"].v);
		    dc.DrawText(s, &cellR,  lowerLeftFlags);
      }

      if (!cell.charEmpty()) { 
	      dc.SelectObject(largeFont);
        const COLORREF txtColor = RGB(255, 255, 255);
        dc.SetTextColor(txtColor);  
		    int px = x * Tiles::TileWidth, py = y * Tiles::TileHeight;
		    CRect cellR( CPoint(px,py), CSize(Tiles::TileWidth,Tiles::TileHeight));
        dc.FillRect(&cellR, &txtBk);
        CString s; s.Format(L"%c", cell.c);
		    dc.DrawText(s, &cellR,  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
      } else { // Only draw light-shading when not text-cell:

        if (!cell.light()) {
          // Base darkening on tile-distance to player:
          int dist = PlayerMob::distPly(CPoint(x, y));
          // Clip dist, for stronger torch:
          dist = dist / 9;
          dist = dist - 2; 
          if (dist < 0) { dist = 0;  }
          int blend = (int) (255.0 - (255.0 / (dist+1)));

          CPoint blendTile(29,20); 
          tiles.drawTileB(x, y, blendTile, dc, true, blend); // 128);
        }
      }

    }
  }

	
}







