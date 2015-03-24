
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
  ::GetCurrentDirectory(sizeof dirBufU/sizeof(TCHAR), dirBufU);

  CT2A dirBufA(dirBufU, CP_ACP);
  // CA2T uc(s.c_str(), CP_ACP); //  CP_UTF8);

  debstr() << "CChildView ctor, cur dir:" << dirBufA << std::endl; 
  // Conclusion: current dir is: 
  // "D:\moria\Basin\src\"

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

	return TRUE;
}


void CChildView::OnTimer(UINT nIDEvent) { // Used to start app loop.
	debstr() << "ontimer:" << nIDEvent << "\n";
	KillTimer(timerID);
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
  Term::term[vp].dirty = true;

  int px = vp.x * Tiles::TileWidth, py = vp.y * Tiles::TileHeight;
	CRect cellR(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));
  invalidateWndJG(&cellR, true); // false); // Is supposed to be 'false' -only experiment, with true.
}


CPoint Viewport::w2v(CPoint w) { return w - offset; }
CPoint Viewport::v2w(CPoint v) { return v + offset; }




class TileDraw {
public:

  void drawTermChar(TCell& tcell) { // CDC& dc, Graphics& gr, CBrush& txtBk, CFont& largeFont, TCell& tcell, int px, int py, int cost) { // CRect& cellR, 
    // Terminal-char cell.
    ++zcost;
    dc.SelectObject(largeFont);

    const COLORREF txtColor = tcell.tcolor; 
    dc.SetTextColor(txtColor);  

    const COLORREF bkColor = tcell.bkcolor; 
    // dc.SetBkColor(bkColor);  

    CRect rect = cellR(); // (CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));

    CBrush txtBk(bkColor); // (RGB(0, 0, 20));
    dc.FillRect(&rect, &txtBk);

    CString s; s.Format(L"%c", tcell.c);
    dc.DrawText(s, &rect,  DT_CENTER | DT_VCENTER | DT_SINGLELINE);
  }


  TileDraw(CDC& dc_, Tiles& tiles_) 
  :dc(dc_), tiles(tiles_) 
  ,gr(dc_)
  // ,txtBk(RGB(0, 0, 20)) 
  {}

  CDC& dc;
  Tiles& tiles;
  Graphics gr; // (dc);
  CFont largeFont, smallFont;


  VPoint vp; // viewport coords.
  CPoint wp; // 'world' (map) coords.
  int zcost; // diagnostics - are we drawing too much.
  int tintCost;
  int px, py;
  CRect cellR_buf;
  const CRect& cellR() const { return cellR_buf; }

  void drawFloorTile(Cell& cell) {
    // DRAW FLOOR:
    tiles.drawTile(vp.p.x, vp.p.y, cell.envir.typeS(), dc, gr, Tiles::Raw, 255, colorNone, zcost, tintCost); // drawing floor.
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
    tiles.drawTile(vp.p.x, vp.p.y, tile, dc, gr, Tiles::Mask, 255, sd.color,zcost, tintCost); // drawing THINGS

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
    tiles.drawTileA(vp.p.x, vp.p.y, cell.creature.typeS(), dc, gr, Tiles::Mask, 255, mobColor, zcost, tintCost); // drawing MOBS

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

    COLORREF darkness = RGB(0, 0, 255);
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
        darkness = colorNone;
      }
    }

    dist = distDyn; if (distStat < dist) { dist = distStat;  }

    int blend = -1;
    if (dist != 0) { //  && !losDark
      blend = dist * 15; // (int)(255.0 - (255.0 / ((0.9*dist) + 1.0)));
      if (blend > 255) { blend = 255;  }

      CPoint blendDarkenTile(29,20); // "Pure black"
      CPoint blendTintTile(36,22); // gray-dither-mass-shadow. 
      ++zcost;
      //Tiles::DrawType transp = Tiles::Blend; // true; // (!losDark && !cell.is_lit());
      if (1) {
        CPoint theTile = (losDark ? blendDarkenTile : blendTintTile);
        tiles.drawTileB(vp.p.x, vp.p.y, theTile, dc, gr, Tiles::Blend /*was:transp true*/, blend, darkness,zcost, tintCost); // shadows-transp.
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


  void doDraw() {

    doFont(largeFont,smallFont, dc);
	  dc.SelectObject(largeFont);

    dc.SetBkMode(TRANSPARENT);

    zcost = 0;
    tintCost = 0;

    for (vp.p.x = 0; vp.p.x < Term::Width; ++vp.p.x) { // Viewport::Width
      for (vp.p.y = 0; vp.p.y < Term::Height; ++vp.p.y) { // Viewport::Height
        TCell& tcell = Term::term[vp.p];
        if (!tcell.dirty && !Term::term.dirtyall) { continue; }
        tcell.dirty = false; // as we are drawing it now, it's no longer dirty.

        // Used by 'all' that follow:
        px = vp.p.x * Tiles::TileWidth, py = vp.p.y * Tiles::TileHeight;
        cellR_buf = CRect(CPoint(px, py), CSize(Tiles::TileWidth, Tiles::TileHeight));

        if (!tcell.charEmpty()) { // If it's a terminal-text-char, just draw that and be done with it:
          drawTermChar(tcell); // dc, gr, txtBk, largeFont, tcell, px, py, cost);
          continue;
        }

        wp = Viewport::vp.v2w(vp.p); // world coords.

        bool losDark = CL->map.lightmap.isDark(wp);
        // (- no, used to..) map (will) return 'nil items' when you ask outside range, because we need to clear/draw outside fields too.
        Cell* pCell = CL->map.cell(wp); 
        if (pCell == NULL  || (losDark && !pCell->is_lit() && !pCell->hasOverlay()) ) { 
          // We MUST draw something for 'dark', otherwise prev.lit tiles will pile up..       
          CPoint darkTile(29,20); // (36, 22);
          tiles.drawTileB(vp.p.x, vp.p.y, darkTile, dc, gr, Tiles::Raw, 255, colorNone,zcost,tintCost); // clearing empty/outside cells.
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
          tiles.drawTileB(vp.p.x, vp.p.y, cell.overlay, dc, gr, Tiles::Mask, 255, colorNone,zcost, tintCost); // overlays/bulletsprites.
        }

        drawLightShadow(cell,losDark); // NB!, this is a major performance hit!
      } // for y.
    } // for x.

    Term::term.dirtyall = false;
    debstr() << "cost:" << zcost << ", tints:" << tintCost << "\n";
  } // doDraw.

}; // end tiledraw.




void CChildView::OnPaint() {
  CRect u; GetUpdateRect(&u);
  if (u.left == 0 && u.bottom > 700) {   // Kludge - idea: if suff. large area is dirty, redraw all..
    Term::term.dirtyall = true;  
    debstr() << u.left << "/" <<  u.top << "/" << u.right << "/" << u.bottom << "\n";
  }

  CPaintDC dc(this); // device context for painting


  TimeMeasure measureDrawing("drawupd");
  // dc.ExcludeUpdateRgn
  TileDraw draw(dc, tiles);
  draw.doDraw(); 
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









