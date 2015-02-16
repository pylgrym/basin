#pragma once

#include <assert.h>

class TCell {
public:
  enum Consts { EmptyChar = '\0' };
  char c;
  TCell() { clearChar(); } // c = '\0'; }
  TCell(char c_):c(c_) {}

  void clearChar() { c = EmptyChar; } // '\0'; }
  bool charEmpty() const { return c == EmptyChar; }
};

class Term
{
public:
  enum { Width = 48, Height = 25 };

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


