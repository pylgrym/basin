#include "stdafx.h"
#include "LOS.h"

// #include "Bresenham.h"

LOS::LOS() {
  // Initial setup.
  initDisted(); // Populates dist-sorted vector, referring to cells from 'cells'.
  popuBehinds(); // draws lines from every point to (0,0), and notes 'behind-cells' on every cell.
  reverseBehinds(); // uses  the 'behinder' data to populate an opposite 'fronts' list on every cell. (every (immediate)in-front cell.)

  // clear();
  propShadows(); // 'push' shadow-info from nearest cell, outwards.
}


LOS::~LOS()
{
}

bool operator < (CPoint a, CPoint b) {
  if (a.x != b.x) { return a.x < b.x;  }
  return  a.y < b.y;  
}


void LOS::test() {
  /* use of propShadows: must be called 8 times.

  oct 0 is ( x,y) ident. (x,y)
  oct 1 is ( y,x: swapped)
  oct 2 is (-y,x) swapped)
  oct 3 is (-x,y)

  oct 4 is (-x,-y) // order?
  oct 5 is (-y,-x)

  oct 6 is ( x,-y)
  oct 7 is ( y,-x)
  */

  deal( 1, 0,    0, 1); //  x, y
  deal( 0, 1,    1, 0); //  y, x

  deal(-1, 0,    0, 1); // -x, y
  deal(0, -1,    1, 0); // -y, x

  deal( 1, 0,    0,-1); //  x,-y
  deal( 0, 1,   -1, 0); //  y,-x

  deal(-1, 0,    0,-1); // -x,-y
  deal( 0,-1,   -1, 0); // -y,-x

}


/* we need a lookup-adapter,
that does coord-offset, and probably (-y,x) transposing.
(I have +x,+y, y <= x coords. in context. plusX, plusY)

class  LTrans {
  CPoint trans(CPoint plus) { // int plusX, int plusY) {
    // int realX, int realY; transp(plusX,plusY, realX, realY)
    CPoint pReal = transp(plus);
    pReal += offset;
    return pReal;
  }

  bool blocked(CPoint plus) {
    CPoint realP = trans(plus);
    return map[realP].envir.blocked();
  }
};

*/

bool LOS::blocked(LCell& c) { // Todo: look up 'is map cell blocked' LOI.
  // (introduce LOI object, so we can query the actual map about blocking cells.)
  return false; 
}


void LOS::deal(int ax, int ay, int bx, int by) { // Idea - test map/los/behinds/fronts? with mouse-over highlights?
  vA.x = ax; vA.y = ay;
  vB.x = bx; vB.y = by;

  clear();
  propShadows(); // 'push' shadow-info from nearest cell, outwards.

  std::vector<LCell*>::iterator i; 
  for (i = disted.begin(); i != disted.end(); ++i) {
    LCell& c = **i; // Now use c.dark:
    // You get 'x'-amount of vA, and 'y'-amount of vB:
    CPoint pA(vA.x*c.x, vA.y*c.x); // FiXME, is this correct?
    CPoint pB(vB.x*c.y, vB.y*c.y);
    CPoint p = pA + pB;

    // todo: Now use 'p' and cell.is-dark:
    c.dark;
    // map[p].dark = c.dark;
  }
}
