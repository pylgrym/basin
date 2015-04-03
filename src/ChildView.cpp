
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
#include "PlayerMob.h"
#include "Dungeons.h"

#include "TimeMeasure.h"


#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView() {
  singletonWnd = this;

  TCHAR dirBufU[256 + 1];
  // what does sizeof actually do? Answer: size in BYTES.
  ::GetCurrentDirectory(sizeof dirBufU / sizeof(TCHAR), dirBufU);

  CT2A dirBufA(dirBufU, CP_ACP);
  // CA2T uc(s.c_str(), CP_ACP); //  CP_UTF8);

  debstr() << "CChildView ctor, cur dir:" << dirBufA << std::endl;
  // Conclusion: current dir is: 
  // "D:\moria\Basin\src\"

  //singletonInit();
}

void CChildView::singletonInit() {
  static bool initYet = false;
  if (initYet) { return; }
  initYet = true;

  int shiftKey = GetAsyncKeyState(VK_SHIFT);
  bool isShiftDown = (shiftKey < 0);
  debstr() << "isShiftDown?" << isShiftDown << "\n";

  // Load normally, re-init if shift is down.
  int answerLoad = (isShiftDown ? IDNO : IDYES); // MessageBox(L"Load?", L"Basin", MB_ICONQUESTION | MB_YESNO);
  Dungeons::the_dungeons.initDungeons(answerLoad == IDYES); // actually, the player..


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
  ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


bool TheUI::hasQuit = false;


bool TheUI::shiftKey() {
  SHORT state = GetAsyncKeyState(VK_SHIFT);
  return (state < 0); // If most significant bit is set, it's negative.
}

bool TheUI::ctrlKey() { return (GetAsyncKeyState(VK_CONTROL) < 0); }


unsigned int TheUI::getNextKey(const char* file, int line, const char* reason) {  // UINT
	// GetMessage loop example. // http://www.cplusplus.com/forum/beginner/38860/	
  if (TheUI::hasQuit) { debstr() << "bail\n";  return VK_CANCEL; }

  debstr() << "WAITING for user keyboard input (getNextKey)" << file << " L" << line << " (" << reason << ")" << std::endl;
	MSG msg;
  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    // debstr() << "msg:" << msg.message << "\n";
    if (msg.message == WM_QUIT || TheUI::hasQuit) {
      debstr() << "TheUI::getNextKey, user is quitting!\n";
      TheUI::hasQuit = true;
      return VK_CANCEL;
    }
		// 	UINT keyCode = msg.wParam;
		// 	return keyCode;
		// }

		TranslateMessage(&msg); // 
		DispatchMessage(&msg);

    if (msg.message == WM_KEYDOWN) { // WM_CHAR) {
      unsigned int charCode = msg.wParam;
      // debstr() << "got key:" << (char) charCode << " " << (void*) charCode << "\n";
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

    if (msg.message == WM_QUIT  || TheUI::hasQuit) {
      debstr() << "TheUI::microSleepForRedraw, user is quitting!\n";
      TheUI::hasQuit = true;
      return VK_CANCEL;
    }

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

  //  | WS_EX_COMPOSITED

	return TRUE;
}


void CChildView::OnTimer(UINT nIDEvent) { // Used to start app loop.
	debstr() << "ontimer:" << nIDEvent << "\n";
  singletonInit();

  if (!CL) { return; } // If we don't have a dungeon yet, don't start any dungeon-stuff..

  KillTimer(timerID);

  const bool wantDoubleBuffer = true; // JG: It actually seems to work.
  if (wantDoubleBuffer && 0) { // 1) { //0) { // 1) {
    // hack to get doublebuffer:  WS_EX_COMPOSITED 
    int style = GetWindowLongPtr( GetSafeHwnd(), GWL_EXSTYLE ); // GetWindowLong
    style |= WS_EX_COMPOSITED;
    SetWindowLongPtr( GetSafeHwnd(), GWL_EXSTYLE, style ); // SetWindowLong
    // 
  }

 
  // do stuff..
  debstr() << "Starting queue-process..\n";
  for (bool isRunning=true; isRunning && !TheUI::hasQuit; ) {
    bool ignored = CL->mobs.dispatchFirst(); //      isRunning = // This was a bug, random mob dying shouldn't throw us out of game-loop..
    // debstr() << "isRunning?" << isRunning << "\n";
    if (PlayerMob::ply->isDead()) {
      logstr log; log << "!You have died!";
      isRunning = false;
    }
  }
  debstr() << "ended queue-process.\n";
}






void doFont(CFont& largeFont, CFont& smallFont, CDC& dc) { // CPaintDC& dc) {
	LOGFONT logFont;
	CFont* pOldFont = dc.GetCurrentFont();
	pOldFont->GetLogFont(&logFont);
	logFont.lfWeight = FW_BOLD;
	logFont.lfWidth = 0;

	// -17 and terminal, 'fits', but is too spaced out.
	// -28 and terminal, has proper widths, but breaks gyjgyj outliers.
	// -24 is a compromise.
  double fontScale = (23.0 / 20.0); // (18.0 / 20.0);
	logFont.lfHeight = (int) (-Tiles::TileWidth * fontScale); //-18; //-24; // 14*1.3f; //-50; //22;
	// const char* fontName = "Terminal"; //"STENCIL"; // "Terminal"
	const TCHAR* fontName2 = L"Rockwell Extra Bold"; //"STENCIL"; // "Terminal"
	const TCHAR* fontName = L"Courier New"; //"STENCIL"; // "Terminal"

  // strncpy_s
	wcsncpy_s(logFont.lfFaceName, sizeof logFont.lfFaceName / sizeof (TCHAR), fontName, sizeof logFont.lfFaceName / sizeof (TCHAR));

	largeFont.CreateFontIndirect(&logFont);

	double smallScale = (6.0/20.0); // was: 9.0
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

  if (pRect == NULL) {
    Term::term.dirtyall = true;
  }
}



void TheUI::invalidateCell(CPoint w_tilepos) { // world coords.
  // (invalidateCell: same as invalidateCellXY, just convenient interface.)
  invalidateCellXY(w_tilepos.x, w_tilepos.y);  
}



void TheUI::invalidateCellXY(int w_tx, int w_ty) { // world coords.
  // (invalidateCellXY: exposed interface func.)
  CPoint wp(w_tx,w_ty); // world

  VPoint vp;
  vp.p = Viewport::vp.w2v(wp); // wp + Viewport::vp.offset;
  invalidateVPCell(vp.p);
}


void TheUI::invalidateTPCell(CPoint tp) { 
  Term::term[tp].dirty = true;

  int px = tp.x * Tiles::TileWidth, py = tp.y * Tiles::TileHeight;
	CRect cellR(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));
  invalidateWndJG(&cellR, true); // false); // Is supposed to be 'false' -only experiment, with true.
}

void TheUI::invalidateVPCell(CPoint vp) { 
  CPoint tp = vp; tp.y += Viewport::Y_Offset;
  Term::term[tp].dirty = true;

  int px = tp.x * Tiles::TileWidth, py = tp.y * Tiles::TileHeight;
	CRect cellR(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));
  invalidateWndJG(&cellR, true); // false); // Is supposed to be 'false' -only experiment, with true.
}


CPoint Viewport::w2v(CPoint w) { return w - offset; }
CPoint Viewport::v2w(CPoint v) { return v + offset; }




class TileDraw {
public:

  TileDraw(CDC& dc_, Tiles& tiles_) 
  :dc(dc_), tiles(tiles_) 
  ,gr(dc_)
  , measure("drawUpd",0)
  // ,txtBk(RGB(0, 0, 20)) 
  {}

  TimeMeasure measure; // ("drawupd", 0);

  CDC& dc;
  Tiles& tiles;
  Graphics gr; // (dc);
  CFont largeFont, smallFont;


  VPoint tp; //terminal points.. (vp-2y)
  VPoint vp; // viewport coords.
  CPoint wp; // 'world' (map) coords.
  int zcost; // diagnostics - are we drawing too much.
  int tintCost;
  int px, py;
  CRect cellR_buf;
  const CRect& cellR() const { return cellR_buf; }
  CPoint mouseTile;


  void drawTermChar(TCell& tcell) { // CDC& dc, Graphics& gr, CBrush& txtBk, CFont& largeFont, TCell& tcell, int px, int py, int cost) { // CRect& cellR, 
    // Terminal-char cell.
    //return;
    ++zcost;
    dc.SelectObject(largeFont);

    const COLORREF txtColor = tcell.tcolor; 
    dc.SetTextColor(txtColor);  

    const COLORREF bkColor = tcell.bkcolor; 
    // dc.SetBkColor(bkColor);  

    CRect rect = cellR(); // (CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));

    CBrush txtBk(bkColor); // (RGB(0, 0, 20));
    dc.FillRect(&rect, &txtBk);

    // Turns out this works, and is necessary:
    char s_ascii[2] = "a"; s_ascii[0] = tcell.c;
    CA2T u_ascii(s_ascii, CP_ACP);
    CString s; s = u_ascii; 
    // s.Format(L"%c", tcell.c); // Does NOT work!

    dc.DrawText(s, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  }

  void drawFloorTile(Cell& cell) {
    // DRAW FLOOR:
    /* fixme - tints are very expensive,
    and for floor-tint, we really should just draw the coloured square instead!
    */
    COLORREF tileColor = cell.envir.ecolor; // typeS() NB/fixme - typeS doesn't work atm anymore?
    // Aach, all the hacks for performance and looks :-(
    /* intention here is, that bare floor is 'just' coloured.
    non-floor, e.g. doors, will be 'mask-transparently' drawn on top of coloured floor:
    */
    tiles.drawTileB(tp.p.x, tp.p.y, cell.envir.tilekey(), dc, gr, Tiles::Raw, 255, tileColor, zcost, tintCost); // drawing floor.
    if (cell.envir.type != EN_Floor) { // plain floor, is just coloured in:
      tiles.drawTileB(tp.p.x, tp.p.y, cell.envir.tilekey(), dc, gr, Tiles::Mask , 255, colorNone, zcost, tintCost); // drawing floor.
    }

    bool floorStat = false; // true;
    if (floorStat) {

      int zeroMobAlert = 0;
      CString s; s.Format(L"%3.0f ",  (double) Mob::noticePlayerProb(wp, zeroMobAlert)); 

      dc.SelectObject(smallFont);
      const int fontFlags = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
      dc.SetTextColor(RGB(255,255,0));  
      CRect lr = cellR();
      dc.DrawText(s, &lr, fontFlags);
    }
  }

  void drawItemTile(Cell& cell) {
    ++zcost;
    CString tile = CA2T(cell.item.atypeS()); // .c_str()
    const SpellDesc& sd = Spell::spell(cell.item.o->effect);
    COLORREF itemColor = sd.color; //colorNone; // sd.color; // colorNone; // sd.color;
    tiles.drawTileU(tp.p.x, tp.p.y, tile, dc, gr, Tiles::Mask, 255, itemColor, zcost, tintCost); // drawing THINGS

    CString s; s.Format(L"<%d>", cell.item.o->ilevel);

    dc.SelectObject(smallFont);
    const int fontFlags = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
    dc.SetTextColor(RGB(0,255,0)); // RGB(0, 0, 255)); // RED.
    CRect lr = cellR();
    dc.DrawText(s, &lr, fontFlags);
  }

  // zeromobalet+

  void drawMobTile(Cell& cell) {
    ++zcost;
    COLORREF mobColor = colorNone;
    const SpellDesc& sd = Spell::spell(cell.creature.m->mobSpell);
    mobColor = sd.color;
    tiles.drawTileA(tp.p.x, tp.p.y, cell.creature.typeS(), dc, gr, Tiles::Mask, 255, mobColor, zcost, tintCost); // drawing MOBS

    // Draw stats/HP:
    Mob* mob = cell.creature.m;
		// int px = vp.p.x * Tiles::TileWidth, py = vp.p.y * Tiles::TileHeight;
    CRect rect = cellR(); // CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight)); // we modify, so need our own.
    CString s; s.Format(L"%d ", mob->stats.hp);

	  dc.SelectObject(smallFont);
    const int fontFlags = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
    dc.SetTextColor(mob->color); // RGB(0, 0, 255)); // RED.
		dc.DrawText(s, &rect,  fontFlags);
    dc.SetTextColor(RGB(0,0,0)); // BLACK.
		rect.OffsetRect(2, 2); dc.DrawText(s, &rect, fontFlags);
    dc.SetTextColor(RGB(255, 255, 255)); // WHITE.
		rect.OffsetRect(-1, -1); dc.DrawText(s, &rect, fontFlags);

    if (false) {
      // Draw level upper right:
      const int upperRightFlags = DT_RIGHT | DT_TOP | DT_SINGLELINE;
      dc.SetTextColor(RGB(0, 255, 0)); // RGB(0, 0, 255)); // RED.
      s.Format(L"%dL", mob->stats.level());
      dc.DrawText(s, &rect, upperRightFlags);
    }


    if (false) {
      // Draw str upper left:
      const int upperLeftFlags = DT_LEFT | DT_TOP | DT_SINGLELINE;
      dc.SetTextColor(RGB(255, 0, 0)); //strength is red.
      s.Format(L"%d", mob->stats.Str.v());
      dc.DrawText(s, &rect, upperLeftFlags);
    }

    if (false) {
      // Draw dex lower left:
      const int lowerLeftFlags = DT_LEFT | DT_BOTTOM | DT_SINGLELINE;
      dc.SetTextColor(RGB(0,0,255)); // dex is blue.
      s.Format(L"%d", mob->stats.Dex.v());
		  dc.DrawText(s, &rect,  lowerLeftFlags);
    }
  }


  // MakeMS-timer
  void drawLightShadow(Cell& cell, bool losDark) {
    /* there is a bug, where permlit areas don't respect gradual lighting, like torch light does. instead, you get an on/off effect. */
    // DARKENING according to light level (we draw 'black darkness' on top of things,
    //  to emulate light/shadow.

    //COLORREF darkness = RGB(0, 0, 255);
    int dist = 999, distStat = 999, distDyn = 999;

    if (cell.is_lit() || cell.hasOverlay()) {
      // Nothing: if cell is perma-lit, don't try to darken it.
      if (!cell.hasOverlay()) { // Then you get relative str of this cells permalight:
        distStat = cell.envir.tmpLightStr;
        //distStat = distStat*distStat;
      } else { // Overlap means brightest light, guaranteed.
        distStat = 0;
      }
    } // was: else

    { // No perma-light in cell:
      // Base darkening on tile-distance to player:
      distDyn = int(0.5+PlayerMob::distPlyLight(CPoint(wp.x, wp.y)));
      if (losDark) { // It should be very totally dark.
        distDyn *= 99;
      } else { // Her er lyst:
        //darkness = colorNone;
      }
    }

    dist = distDyn; if (distStat < dist) { dist = distStat;  }

    int blend = -1;
    if (dist != 0) { //  && !losDark
      blend = dist * 15; // (int)(255.0 - (255.0 / ((0.9*dist) + 1.0)));
      if (blend > 255) { blend = 255;  }

      // JG: (36,22 is dither-tile. 29-20 is 'pure black'.)
      CPoint blendDarkenTile(29, 20); // 27, 22); /// 29, 20); // "Pure black" (we never get to see this one..)
      CPoint blendTintTile(29, 20); // (36, 22); // gray-dither-mass-shadow. // this one always wins, currently.
      ++zcost;
      //Tiles::DrawType transp = Tiles::Blend; // true; // (!losDark && !cell.is_lit());
      if (1) {
        CPoint theTile = (losDark ? blendDarkenTile : blendTintTile);
        tiles.drawTileB(tp.p.x, tp.p.y, theTile, dc, gr, Tiles::Blend /*was:transp true*/, blend, colorNone,zcost, tintCost); // shadows-transp.
      } else { // try simpler shading.
        // JG: Actually, doesn't help at all.
        CRect r = cellR();  
        Gdiplus::Rect r2(r.left, r.top, r.Width(), r.Height());

        Gdiplus::Color alf(blend, 5, 10, 15); // = Color::FromArgb(blend, 5, 10, 15);
        SolidBrush alfBrush(alf); // = new SolidBrush(clr);
        gr.FillRectangle(&alfBrush, r2);

        // Won't work - no alpha channel in basic GDI :-(
        // COLORREF transColor = RGB(5, 10, 15, blend);
        // dc.FillSolidRect(&rect, transColor);
      }
      /* IDEA: Instead, make a stand-alone app that solely does drawing and timing,
      no complicated code whatsoever..
      */
    }

    if (0) { // 1) { // true) {
      dc.SelectObject(smallFont);
      CRect rect = cellR();
      dc.SetTextColor(RGB(255,255,255)); CString s; //white: dist
      s.Format(L" %d", dist); const int lowerLeftFlags = DT_LEFT | DT_BOTTOM | DT_SINGLELINE;
		  dc.DrawText(s, &rect,  lowerLeftFlags);

      dc.SetTextColor(RGB(255,0,0));  // red: blend
      s.Format(L"%d ", blend); const int flags4 = DT_LEFT | DT_TOP | DT_SINGLELINE;
		  dc.DrawText(s, &rect,  flags4);

      if (0) {
        //              // yell: distStat
        dc.SetTextColor(RGB(255,255,0));  // yel: dist
        s.Format(L"%d ", distStat); const int flags2 = DT_RIGHT | DT_TOP | DT_SINGLELINE;
		    dc.DrawText(s, &rect,  flags2);

        dc.SetTextColor(RGB(0,255,0));  // green: distdyn
        s.Format(L"%d ", distDyn); const int flags3 = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
		    dc.DrawText(s, &rect,  flags3);

        dc.SetTextColor(RGB(255,0,0));  // red: permlight
        s.Format(L"%d ", cell.envir.permLight); const int flags4 = DT_LEFT | DT_TOP | DT_SINGLELINE;
		    dc.DrawText(s, &rect,  flags4);

        dc.SetTextColor(RGB(0,0,255));  // blue: losDark-bool
        s.Format(L" %d ", losDark); const int flags5 = DT_CENTER |  DT_VCENTER | DT_SINGLELINE;
		    dc.DrawText(s, &rect,  flags5);
      }
    }

  } // end drawLightShadow.



  std::string hoverInfo2;

  void newDraw() {
    /* goal here is to make the tighest fastest loop that 'gets the job done',
    to demonstrate a 'lower bound' for how fast/slow this can be done..
    */
    doFont(largeFont,smallFont, dc);
    dc.SetBkMode(TRANSPARENT);

    const CPoint greyDitherTile(36, 22), greyTile2(36,21);

    for (tp.p.x = 0; tp.p.x < Term::Width; ++tp.p.x) { // Viewport::Width
      for (tp.p.y = 0; tp.p.y < Term::Height; ++tp.p.y) { // Viewport::Height
        TCell& tcell = Term::term[tp.p];
        if (!tcell.dirty && !Term::term.dirtyall) { continue; }
        tcell.dirty = false; // as we are drawing it now, it's no longer dirty.

        // Used by 'all' that follow:
        px = tp.p.x * Tiles::TileWidth, py = tp.p.y * Tiles::TileHeight;
        cellR_buf = CRect(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));

        if (!tcell.charEmpty()) { // If it's a terminal-text-char, just draw that and be done with it:
          drawTermChar(tcell); // dc, gr, txtBk, largeFont, tcell, px, py, cost);
          continue;
        }

        if (tp.p.y < Viewport::Y_Offset) {
          continue; // first two rows are terminal only, not part of viewport.
        }

        vp.p.x = tp.p.x; vp.p.y = tp.p.y - Viewport::Y_Offset;

        wp = Viewport::vp.v2w(vp.p); // world coords.

        CPoint floortile = greyDitherTile; // By default, assume any cell is gray/dark.

        bool losDark = CL->map.lightmap.isDark(wp);
        Cell* pCell = NULL;
        if (!losDark) {
          pCell = CL->map.cell(wp); 
          if (pCell != NULL) { floortile = pCell->envir.tilekey(); }
        }

        if (vp.p == mouseTile) { floortile = greyTile2; }

        tiles.drawTileB(tp.p.x, tp.p.y, floortile, dc, gr, Tiles::Raw, 255, colorNone,zcost,tintCost); // draw envir.
        // 15-30 ms for this part..

        bool litCell = false;
        if (!losDark && pCell != NULL) { // nothing visible to draw.
          Cell& c = *pCell; // We are certain we have a cell, and it's in light.
          litCell = c.is_lit();

          if (!c.item.empty()) {
            const SpellDesc& sd = Spell::spell(c.item.o->effect);
            COLORREF color = colorNone; // sd.color; // colorNone; // sd.color;
            CPoint itemTile = c.item.tilekey();
            tiles.drawTileB(tp.p.x, tp.p.y, itemTile, dc, gr, Tiles::Mask, 255, color, zcost, tintCost); // draw item.
            if (tp.p == mouseTile) { hoverInfo2 = c.item.atypeS(); }
          }

          if (!c.creature.empty()) {
            CPoint mobTile = c.creature.mtilekey();
            tiles.drawTileB(tp.p.x, tp.p.y, mobTile, dc, gr, Tiles::Mask, 255, colorNone, zcost, tintCost); // draw mob.
          }
        } // draw if cell


        // now darken:
        int dist = int(0.5+PlayerMob::distPlyLight(CPoint(wp.x, wp.y)));
        int blend = dist * 15; 
        if (blend > 255) { blend = 255;  }
        if (litCell && losDark) { blend = 128; }

        if ( (dist != 0 && !losDark) || litCell) {
          tiles.drawTileB(tp.p.x, tp.p.y, greyDitherTile, dc, gr, Tiles::Blend, blend, colorNone,zcost,tintCost); // draw darkening.
        }

        if (pCell != NULL && pCell->hasOverlay()) { // draws bullet sprites, spell effects, rain etc.
          tiles.drawTileB(tp.p.x, tp.p.y, pCell->overlay, dc, gr, Tiles::Mask, 255, colorNone,zcost, tintCost); // overlays/bulletsprites.
        }

      }
    }
    // reportCost();
  }
  


  void doDraw() {
    // newDraw(); return;

    doFont(largeFont,smallFont, dc);
	  dc.SelectObject(largeFont);

    dc.SetBkMode(TRANSPARENT);

    zcost = 0;
    tintCost = 0;

    for (tp.p.x = 0; tp.p.x < Term::Width; ++tp.p.x) { // Viewport::Width
      for (tp.p.y = 0; tp.p.y < Term::Height; ++tp.p.y) { // Viewport::Height
        TCell& tcell = Term::term[tp.p];
        if (!tcell.dirty && !Term::term.dirtyall) { continue; }
        tcell.dirty = false; // as we are drawing it now, it's no longer dirty.

        // Used by 'all' that follow:
        px = tp.p.x * Tiles::TileWidth, py = tp.p.y * Tiles::TileHeight;
        cellR_buf = CRect(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));

        if (!tcell.charEmpty()) { // If it's a terminal-text-char, just draw that and be done with it:
          drawTermChar(tcell); // dc, gr, txtBk, largeFont, tcell, px, py, cost);
          continue;
        }

        if (!CL) { continue; } // Don't draw other parts, if we don't have a dungeon..

        if (tp.p.y < Viewport::Y_Offset) {
          continue; // first two rows are terminal only, not part of viewport.
        }

        vp.p.x = tp.p.x; vp.p.y = tp.p.y - Viewport::Y_Offset;

        wp = Viewport::vp.v2w(vp.p); // world coords.

        bool losDark = CL->map.lightmap.isDark(wp);
        if (PlayerMob::ply->overrideLight) { losDark = false; } //A '*' toggle to light everything.

        // (- no, used to..) map (will) return 'nil items' when you ask outside range, because we need to clear/draw outside fields too.
        Cell* pCell = CL->map.cell(wp); 
        if (pCell == NULL  || (losDark && !pCell->is_lit() && !pCell->hasOverlay()) ) { 
          // We MUST draw something for 'dark', otherwise prev.lit tiles will pile up..       
          CPoint darkTile(29,20); // (36, 22);
          tiles.drawTileB(tp.p.x, tp.p.y, darkTile, dc, gr, Tiles::Raw, 255, colorNone,zcost,tintCost); // clearing empty/outside cells.
          continue;
        }

        Cell& cell = *pCell; 
        drawFloorTile(cell); // DRAW FLOOR. (and possibly some debug stats.)

        if (!cell.item.empty()) { // DRAW ITEM:
          drawItemTile(cell);
        }

        if (!cell.creature.empty() && !losDark) { // DRAW MOB (and possibly some debug-stats.)
          drawMobTile(cell); // note we only draw mob is not los-dark.. (equals LOS to player.)
        }

        if (cell.hasOverlay()) { // draws bullet sprites, spell effects, rain etc.
          ++zcost;
          tiles.drawTileB(tp.p.x, tp.p.y, cell.overlay, dc, gr, Tiles::Mask, 255, colorNone,zcost, tintCost); // overlays/bulletsprites.
        }

        drawLightShadow(cell,losDark); // NB!, this is a major performance hit!
      } // for y.
    } // for x.

    Term::term.dirtyall = false;
    // reportCost();
  } // doDraw.

  void reportCost() {
    int delta = measure.stopClock();
    debstr() << delta << "(ms-time), cost:" << zcost << ", tints:" << tintCost << "\n";
  }

}; // end tiledraw.




void CChildView::OnPaint() {
  singletonInit(); // actually this seems to be called first.

  CRect u; GetUpdateRect(&u);
  if (u.left == 0 && u.bottom > 700) {   // Kludge - idea: if suff. large area is dirty, redraw all..
    Term::term.dirtyall = true;  
    //debstr() << u.left << "/" <<  u.top << "/" << u.right << "/" << u.bottom << "\n";
  }

  CPaintDC dc(this); // device context for painting


  // dc.ExcludeUpdateRgn
  TileDraw draw(dc, tiles);
  draw.mouseTile = mouseTile;

  // draw.newDraw(); // 28ms
  draw.doDraw(); 

  if (draw.zcost > 3) { // Don't spam out reports on tiny-redraws.
    draw.reportCost();
  }

  int delta = draw.measure.getDelta(); // stopClock();
  std::stringstream ss; 
  ss << delta << "/" << draw.measure.start << "/" << draw.measure.end; 
  std::string s = ss.str();

  CA2T us(s.c_str());
  // CA2T us(draw.hoverInfo2.c_str());
  GetParent()->SetWindowText(us); // setwindowtitle.
}




void tintTileDemoCode(CRect& dest, Gdiplus::Graphics& graphics, COLORREF matColor) {
  Gdiplus::Image sprites2(L"tiles.png"); // (L"potion.png"); // RotationInput.bmp");
  //:sprites2(tileFile) 

  // COLORREF matColor = colors[matIndex].color;
  int col = 0, row = 0;
  int cellWidth = 32, cellHeight = 32;
  int offset_x = col * cellWidth;
  int offset_y = row * cellHeight;

  Gdiplus::Rect dest2( dest.left,dest.top,  dest.Width(),dest.Height()); 

  Color tintingColor( GetRValue(matColor), GetGValue(matColor), GetBValue(matColor) ); // 255, 0, 0);
  // int angle = 154;
	// tintingColor.SetValue( Color::MakeARGB(255, angle, 255-angle, tintingColor.GetBlue() )); // tintingColor.GetGreen()

	float cr = tintingColor.GetRed()   / 255.0f;
  float cg = tintingColor.GetGreen() / 255.0f;
  float cb = tintingColor.GetBlue()  / 255.0f;

  ColorMatrix colorMatrix = {
    // 1,0,0,0,0, 
    cr, cg, cb, 0, 0,
    // 0,1,0,0,0, 
    cb, cr, cg, 0, 0,
    // 0,0,1,0,0, 
    cg, cb, cr, 0, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 0, 1
  }; 
  ImageAttributes  imageAttributes;

	imageAttributes.SetColorMatrix(
	   &colorMatrix, 
	   ColorMatrixFlagsDefault,
	   ColorAdjustTypeBitmap);   


  // UINT             zzwidth = sprites2.GetWidth();
	graphics.DrawImage(
	   &sprites2, 
	   dest2, // Rect(32, 5, width, height),  // destination rectangle 
	   offset_x, offset_y, // 0, 0,        // upper-left corner of source rectangle 
	   cellWidth, // width,       // width of source rectangle
	   cellHeight, // height,      // height of source rectangle
	   UnitPixel,
	   &imageAttributes
  );

}











void CChildView::OnMouseMove(UINT nFlags, CPoint point) {
  CWnd::OnMouseMove(nFlags, point);

  //mousepos = point;

  CPoint oldTile = mouseTile;
  mouseTile = CPoint(point.x / 32, point.y / 32);
  TheUI::invalidateTPCell(mouseTile);
  TheUI::invalidateTPCell(oldTile);
}
