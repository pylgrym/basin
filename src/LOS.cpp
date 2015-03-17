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

  deal( 0, 1,    1,0);
  deal( 1, 0,    0,1);

  deal(0, -1,    1,0);
  deal(-1, 0,    0,1);

  deal( 0, 1,   -1, 0);
  deal( 1, 0,    0,-1);

  deal( 0,-1,   -1, 0);
  deal(-1, 0,    0,-1);

}



void LOS::deal(int ax, int ay, int bx, int by) {
  vA.x = ax; vA.y = ay;
  vB.x = bx; vB.y = by;
}
