#include "stdafx.h"
#include "Cuss.h"

// #include "cellmap/cellmap.h"

#include "term.h"

#include "theUI.h"

CPoint Cuss::csr = CPoint(0, 0);  


void Cuss::clear(bool bInvalidate) { 
   
  csr = CPoint(0, 0);  
  // clear map:
  for (int x = 0; x < Term::Width; ++x) {
    //CellColumn& column = CL->map[x];
    for (int y = 0; y < Term::Height; ++y) {
      CPoint p(x,y);
      TCell& cell = Term::term[p]; //column[y];
      cell.clearChar();
    }
  }
  if (bInvalidate) {
    TheUI::invalidateWndJG(NULL, true);
  }
}


void Cuss::prtL(const char* txt, bool stay) {
  CPoint origin = csr;
  csr.x = 0;
  bool bClipped = prt(txt, true);
  while (!bClipped) {
    bClipped = putchar(' ', true);
  }
  // Note that this automatically achieves a CR-LF, without calling  CRLF (because we print up to the edge.)

  if (stay) { csr = origin;  } // Move cursor back where we started.
}


bool Cuss::prt(const char* txt, bool bDoClip) {
  int count = 0; // This is not 'end of line' check, more like 'not too many chars' check.
  bool bClipped = false;
  for (const char* s = txt; *s != '\0' && count < Term::Width && !bClipped; ++s, ++count) {
    bClipped = putchar(*s, bDoClip);
  }
  return bClipped;
}


bool Cuss::prtCR(const char* txt, bool bDoClip) {
  bool bClipped = prt(txt, bDoClip);
  CRLF();
  return bClipped;
}

void Cuss::CRLF() { // Do carriage-return + lineFeed.
  csr.x = 0;
  ++csr.y;
  if (csr.y >= Term::Height) { csr.y = 0;  }
}


void Cuss::setTxtColor(COLORREF tcol) { curTxtColor = tcol; }
COLORREF Cuss::curTxtColor = RGB(255,255,255);


bool Cuss::putchar(char c, bool bClip) {
  // debstr() << "[§" << c << "]\n";
  TCell& cell = Term::term[csr]; //CL->map[csr];
  cell.c = c; cell.tcolor = curTxtColor;
  TheUI::invalidateVPCell(csr);

  ++csr.x;
  if (csr.x >= Term::Width) { // Map::Width) {
    csr.x = 0;
    ++csr.y;
    if (csr.y >= Term::Height) { // Map::Height) { 
      csr.y = 0;  
    }

    if (bClip) {
      return true;
    }
  }
  return false;
}

void Cuss::invalidate() {
  TheUI::invalidateWndJG(NULL, true);
}


int TheUI::waitForKey(const char* file, int line, const char* reason) { 
  return getNextKey(file, line, reason); // in waitForKey.
}

int TheUI::promptForAnyKey(const char* file, int line, const char* reason) {
  Cuss::prtL("  Press any key.");
  return getNextKey(file, line, reason); // in promptForAnyKey.
}

int TheUI::promptForKey(const char* prompt, const char* file, int line, const char* reason) {
  Cuss::prtL(prompt, true);
  int key = getNextKey(file, line, reason); // in promptForKey.
  return key;
}


void Cuss::move(CPoint pos) {
  csr = pos;
}


/*
Cuss::Cuss()
{
}


Cuss::~Cuss()
{
}
*/