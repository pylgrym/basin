#pragma once

#include <vector>
#include <assert.h>

#include "numutil/myrnd.h"
// #include "Kaching.h" // for RND..



class BungBrownian
{
public:
  BungBrownian();
  ~BungBrownian();
};


struct BrCell {
  enum BrownEnums {
    BE_Empty = 0,
    BE_Hall,
    BE_Room,
    BE_Floor,
    BE_Wall,
  };
  int c;
  BrCell() :c(BE_Empty) {}
};


class Brown {
public:
  const int BrWidth, BrHeight;
  std::vector< std::vector <BrCell> > cells_i;

  CDC* dc;

  Brown(int width, int height, CDC* dc_)
    :BrWidth(width), BrHeight(height), dc(dc_)
  {
    cells_i.resize(BrWidth);

    for (int column = 0; column < BrWidth; ++column){
      cells_i[column].resize(BrHeight);
    }
  }

  void normCoord(LONG& c) {
    if (c != 0) { c = c / abs(c); }
  }

  CPoint norm(CPoint v) {
    normCoord(v.x);
    normCoord(v.y);
    return v;
  }

  void rect(CPoint a, CPoint b, int val) {
    CRect r(a, b);
    r.NormalizeRect();
    CPoint ul = r.TopLeft();
    CPoint ur = ul;
    ur.x = r.right;
    CPoint L = ul;
    CPoint R = ur;
    for (int i = 0; i < r.Height(); ++i) {
      line(L, R, val);
      ++L.y; ++R.y;
    }

    CPoint br = r.BottomRight();
    CPoint bl = br; bl.x = r.left;
    line(ul, ur, BrCell::BE_Wall);
    line(ul, bl, BrCell::BE_Wall);
    line(ur, br, BrCell::BE_Wall);
    line(bl, br, BrCell::BE_Wall);
  }

  void line(CPoint a, CPoint b, int val) {
    CPoint delta = b - a;
    CPoint dir = norm(delta);
    for (CPoint p = a;; p += dir) {
      point(p, val);
      if (p == b) { break; }
    }
  }

  void point(CPoint p, int val) {
    p.x = p.x % BrWidth;
    p.y = p.y % BrHeight;
    cell(p).c = val;
    if (dc != NULL) {
      drawPoint(p, val);
    }
  }

  void drawPoint(CPoint p, int val) {
    const int side = 8;
    CPoint ul(p.x*side, p.y*side);
    CRect r(ul, CSize(side, side));
    COLORREF color = RGB(255, 255, 128);
    switch (val) {
    case BrCell::BE_Floor: color = RGB(255, 255, 240); break; // interior is white.
    case BrCell::BE_Wall: color = RGB(0, 32, 16); break; // Room-walls are black.
    case BrCell::BE_Hall: color = RGB(0, 32, 255); break; // corridors are blue.
    default: color = RGB(255, 0, 0); break; // mistakes are red..
    }
    CBrush b(color); // RGB(128, 192, 255));
    dc->FillRect(&r, &b);
  }

  BrCell& cell(CPoint p2) {
    CPoint p = p2;
    p.x = p.x % BrWidth;
    p.y = p.y % BrHeight;
    if (p.x < 0) { p.x += BrWidth; }
    if (p.y < 0) { p.y += BrHeight; }

    if (!(p.x >= 0)) {
      DebugBreak();
    }
    assert(p.x >= 0);

    if (!(p.y >= 0)) {
      DebugBreak();
    }
    assert(p.y >= 0);

    if (!(p.x < BrWidth)) {
      DebugBreak();
    }
    assert(p.x < BrWidth);

    if (!(p.y < BrHeight)) {
      DebugBreak();
    }
    assert(p.y < BrHeight);
    return cells_i[p.x][p.y];
  }

  void run() { //CDC& dc) {
    // CBrush brush(RGB(240, 240, 240));

    CPoint p(BrWidth / 2, BrHeight / 2); // 250, 250);

    for (int i = 0; i < 50; ++i) {

      int dirB = rnd::Rnd(2) ? 1 : -1;
      int dir1 = dirB * rnd::Rnd(7, 20);
      int xy = rnd::Rnd(2);
      CPoint dir(xy ? dir1 : 0, !xy ? dir1 : 0);

      CPoint next = p + dir;
      doCorridor(p, next);

      CSize dims(rnd::Rnd(2, 10), rnd::Rnd(2, 10));
      CPoint center(dims.cx / 2, dims.cy / 2);
      CRect r(CPoint(next.x - center.x, next.y - center.y), dims);
      rect(r.TopLeft(), r.BottomRight(), BrCell::BE_Floor); // dc.FillRect(&r, &brush);
      p = next;
    }
    refreshCorridors();
  }

  void doCorridor(CPoint p, CPoint next) {
    line(p, next, BrCell::BE_Empty); // val); // dc.MoveTo(p); dc.LineTo(next);
    CRect corr(p, next);
    corridors.push_back(corr);
  }

  std::vector< CRect > corridors;

  void refreshCorridors() {
    std::vector< CRect >::iterator i;
    for (i = corridors.begin(); i != corridors.end(); ++i) {
      CRect corr = *i;
      line(corr.TopLeft(), corr.BottomRight(), BrCell::BE_Hall);
    }
  }

};
