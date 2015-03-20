#include "stdafx.h"
#include "CanSee.h"
#include "Bresenham.h"
#include "cellmap/cellmap.h"


CanSee::CanSee()
{
}


CanSee::~CanSee()
{
}

bool CanSee::canSee(CPoint a, CPoint b, class Map& map, bool onlyEnvir) {
  /* JG, consider: we might want different variants - seeing through walls versus seeing through other mobs.
  use this to check if LOS-can-see for spell-cast.
  */
  BresIter i(a, b); 
  for (; !i.done();) {
    CPoint p = i.next();
    Cell& c = map[p];
    if (c.envir.blocked() || (!onlyEnvir && c.blocked())) { 
      return false; 
    }
  }
  return true;
}
