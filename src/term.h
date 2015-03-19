#pragma once

#include <assert.h>

class TCell {
public:
  enum Consts { EmptyChar = '\0' };
  char c;
  bool dirty;
  COLORREF tcolor,bkcolor;
  TCell() { clearChar(); } // c = '\0'; }
  TCell(char c_):c(c_), dirty(true),tcolor(RGB(255,255,0)), bkcolor(RGB(128,0,0)) {}

  void clearChar() { c = EmptyChar; tcolor = RGB(255, 255, 255); bkcolor = RGB(0, 32, 0); dirty = true;  } // Consider if def tcolor can be set, to e.g. green.
  bool charEmpty() const { return c == EmptyChar; }
};

class Term
{
public:
  enum { 
	  Width = 48, 
	  Height = 20 // 25 
  };

  bool dirtyall;
  TCell cells[Width][Height];

  TCell& operator[] (CPoint tp) {
    static TCell none('§');
    if (tp.x < 0 || tp.x >= Width || tp.y < 0 || tp.y >= Height) { return none; }
    assert(tp.x >= 0);
    assert(tp.x < Width);
    assert(tp.y >= 0);
    assert(tp.y < Height);
    return cells[tp.x][tp.y];
  }

  Term(void);
  ~Term(void);

  static Term term;
};


