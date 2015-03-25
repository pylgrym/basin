#include "stdafx.h"
#include "LOS.h"
#include "cellmap/cellmap.h"
#include "./theUI.h"

// #include "Bresenham.h"

LOS::LOS() {
  // Initial setup.
  initDisted(); // Populates dist-sorted vector, referring to cells from 'cells'.
  popuBehinds(); // draws lines from every point to (0,0), and notes 'behind-cells' on every cell.
  reverseBehinds(); // uses  the 'behinder' data to populate an opposite 'fronts' list on every cell. (every (immediate)in-front cell.)
}




LOS::~LOS()
{
}

LOS LOS::los;
// LightMap  static LightMap lightmap;


bool operator < (CPoint a, CPoint b) {
  if (a.x != b.x) { return a.x < b.x;  }
  return  a.y < b.y;  
}



void LightMap::invalidateDiff(LightMap& newLM) {
  LightMap& old = *this;
  int losInvalidate = 0;
  for (int x = -SidePart-1; x <= SidePart+1; ++x) {
    for (int y = -SidePart-1; y <= SidePart+1; ++y) {
      CPoint p(x, y);
      p += newLM.map_offset;
      if (old.isDark(p) != newLM.isDark(p)) {
        TheUI::invalidateCell(p);
        ++losInvalidate;
      } else if (!newLM.isDark(p)) { // JG: - I realise, that for proper shading, still-lighted cells must be invalidated too.
        TheUI::invalidateCell(p);
        ++losInvalidate;
      }
    }
  }
  //debstr() << "Los-Inval#:" << losInvalidate << "\n";
}


void LOS::recalcLOS(LightMap& lm) {
  popuLightMap(lm); // 'push' shadow-info from nearest cell, outwards.
}


void LOS::popuLightMap(LightMap& light) {

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

  doOctant(light, 1, 0,    0, 1); //  x, y
  doOctant(light, 0, 1,    1, 0); //  y, x

  doOctant(light,-1, 0,    0, 1); // -x, y
  doOctant(light,0, -1,    1, 0); // -y, x

  doOctant(light, 1, 0,    0,-1); //  x,-y
  doOctant(light, 0, 1,   -1, 0); //  y,-x

  doOctant(light,-1, 0,    0,-1); // -x,-y
  doOctant(light, 0,-1,   -1, 0); // -y,-x

  // Now, isDark properties of lightmap have been filled out.
}



void LOS::doOctant(LightMap& light, int ax, int ay, int bx, int by) { // Idea - test map/los/behinds/fronts? with mouse-over highlights?
  bool withDiag = (ax != 0); // The (x,y)'s include the diag, the (y,x) skip the diag. (to avoid doing diag twice.)

  vA.x = ax; vA.y = ay;
  vB.x = bx; vB.y = by;

  clear(); // We must re-clear it for every octant.
  propShadows(light); // 'push' shadow-info from nearest cell, outwards.

  // Now transfer dark/light markings, from 'disted LCell' structure, to the LightMap:
  std::vector<LCell*>::iterator i; 
  for (i = disted.begin(); i != disted.end(); ++i) {
    LCell& c = **i; // Now use c.dark:
    if (c.x == c.y && !withDiag) { continue; }

    CPoint pA(vA.x*c.x, vA.y*c.x); // You get 'x'-amount of vA, and 'y'-amount of vB:
    CPoint pB(vB.x*c.y, vB.y*c.y);
    CPoint rel = pA + pB;

    light.useDark(rel, c); // (transfer LCell.dark to 'rel' point in map. 
  }
}

void LightMap::useDark(CPoint rel, LCell& cell) {
  // (NB: Because we transfer to all cells in lightmap, we don't have to clear the lightmap.)
  dark(rel) = cell.dark; // LightMap has a zero-centered (-Side,+Side) coord-sys.
}


bool LOS::blocked(LCell& c, LightMap& light) { // Todo: look up 'is map cell blocked' LOI.
  // 'c' is a (x,y) (octant 0) point, we'll transform it to any octant, with our vector-base:
  CPoint pA(vA.x*c.x, vA.y*c.x); // You get 'x'-amount of vA, and 'y'-amount of vB:
  CPoint pB(vB.x*c.y, vB.y*c.y);
  CPoint rel = pA + pB; // NB, It's still (0,0) based.

  bool isBlocked = light.isBlocked(rel);
  return isBlocked;
}

bool LightMap::isBlocked(CPoint rel) {
  // (Demeter is crying, about all my isblocked' delegation?)
  // (LightMap: introduced LOI object, so we can query the actual map about blocking cells.)
  CPoint mapPoint = (map_offset + rel);
  if (!map->legalPos(mapPoint)) { return true; } // If it's outside map, it's blocked..
  Cell& mapCell = (*map)[mapPoint];
  return mapCell.envir.blocked(); 
}


bool LightMap::legalRelPos(CPoint rel) const { 
  bool xOK = (rel.x >= -SidePart && rel.x <= SidePart);
  bool yOK = (rel.y >= -SidePart && rel.y <= SidePart);
  return xOK && yOK;
}

bool LightMap::isDark(CPoint mapPoint) { // uses map_offset.
  CPoint rel = (mapPoint - map_offset);
  if (!legalRelPos(rel)) { return true;  }
  bool isItDark = dark(rel); 
  return isItDark;
}


void LOS::propShadows(LightMap& light) {
  // Consider: in theory, propShadows might also use 'bool withDiag', to skip doing diag twice,
  // but OTOH, I wonder if it would cause assymetrical artifacts from 'side shadows'..?

  std::vector<LCell*>::iterator i; 
  for (i = disted.begin(); i != disted.end(); ++i) {
    LCell& c = **i;
    bool isBlocked = blocked(c, light);
    // NB! 'is-blocked' puts your BEHINDS in shadow, but not yourself!
    if (c.dark || isBlocked) {
      std::set<CPoint>::iterator pi;
      for (pi = c.behinds.begin(); pi != c.behinds.end(); ++pi) {
        CPoint behind = *pi;
        LCell& cBehind = cell(behind);
        cBehind.dark = true;
      }
    }
  }
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
