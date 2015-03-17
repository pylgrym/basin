#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <assert.h>

#include "Bresenham.h"

extern bool operator < (CPoint a, CPoint b);

class LightMap {
public:
  enum Sizes { SidePart = 20, Side = SidePart * 2 + 1 }; // FIXME, call them 'SideHalf' instead.

  // Ctor to clear/init darks contents.
  LightMap():map(NULL) { clear(); } // NB, map ptr is initialized in ugly way.

  // static LightMap lightmap; // NO, put it in map/dungeon instead.
  CPoint map_offset; // Origo-player pos.
  class Map* map; // Used to check if a cell is blocked/casting shadows.

  void clear() {
    for (int x = -SidePart; x <= SidePart; ++x) {
      for (int y = -SidePart; y <= SidePart; ++y) {
        CPoint p(x, y);
        dark(p) = false;
      }
    }
  }

  bool darks[Side][Side];

  bool& dark(CPoint p) {
    CPoint center(SidePart, SidePart);
    CPoint i = p + center;
    assert(i.x >= 0);
    assert(i.y >= 0);
    assert(i.x < Side);
    assert(i.y < Side);
    return darks[i.x][i.y];
  }

  void useDark(CPoint rel, class LCell& cell); // Takes note, whether relative-point is in shadow or not.
  bool isBlocked(CPoint rel); // Checks the map, if relative-point is blocked or not.
  bool isDark(CPoint mapPoint); // uses map_offset.
  bool legalRelPos(CPoint rel) const;

};

class LCell : public CPoint {
public:
  LCell() { ix = 0; distsq = 0; dark = false; } // visited = false;

  int ix;
  // int x, y, ix;

  int distsq;
  int calcdistsq() const { return x*x + y*y; }

  bool dark; // in shadow.
  void clear() {
    dark = false; // So far, we only have the 'dark' flag, no 'visited' flag.
  }
  // bool visited;
  // These are only populated in some cases.
  std::set<CPoint> behinds; // all points immediately behind me.
  std::set<CPoint> fronts; // all points immediately in front of me.
};

class NearOrder { // distance-based sorting.
public:
  bool operator () (LCell* L, LCell* R) { return L->distsq < R->distsq;  }
};



class LOS
{
public:
  enum Sizes { Side = 20};
  LCell cells[Side][Side];
  std::vector<LCell*> disted;

  LCell& cell(CPoint p) { 
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.y <= p.x); // only allow lower diagonal.
    return cells[p.x][p.y];
  }

  void initDisted() {
    int ix = 0;
    for (int x = 0; x < Side; ++x) {
      for (int y = 0; y <= x; ++y, ix++) {
        LCell& c = cells[x][y];
        c.x = x; c.y = y; c.ix = ix;
        c.distsq = c.calcdistsq();
        disted.push_back(&c);
      }
    }
    std::sort(disted.begin(), disted.end(), NearOrder() );
  }


  void reverseBehinds() { // create fronts, based on behinds.

    std::vector<LCell*>::iterator i; 
    for (i = disted.begin(); i != disted.end(); ++i) {
      LCell& cFront = **i;
      std::set<CPoint>::iterator pi;
      for (pi = cFront.behinds.begin(); pi != cFront.behinds.end(); ++pi) {
        CPoint behind = *pi;
        LCell& cBehind = cell(behind);
        cBehind.fronts.insert(cFront);
      }
    }

  } // reverseBehinds.


  LOS();
  ~LOS();

  static LOS los;


  void popuBehinds() {
    /* 
    fixme: can we speed this up, skip any parts?
    right now, we draw lines to everywhere, and 

    fixme: establish what 'visited' really means
     - which invariants we are trying to establish,
     and what the various processing steps are supposed to do.
    
     - which kinds of 'visits' do we allow from several directions?
    */

    // Iterate through cells, furthest first.
    std::vector<LCell*>::reverse_iterator i; 
    for (i = disted.rbegin(); i != disted.rend(); ++i) {
      LCell& c = **i;
      // if (c.visited) { continue; }
      // c.visited = true;

      //Loop through all pixels, farthest to nearest:
      CPoint ep1(c); CPoint ep2(0, 0);
      BresIter bres(ep1, ep2); bres.iterAll1();

      CPoint prev;
      std::vector<CPoint>::iterator pi;
      for (pi = bres.pixels.begin(); pi != bres.pixels.end(); ++pi) {
        CPoint p = *pi;
        LCell& c = cell(p);
        //if (c.visited) { continue;  }
        if (pi != bres.pixels.begin()) { // Skip the first iteration.
          c.behinds.insert(prev);
        }
        prev = p;
      }
    }
    // for (int ix = 0; ix < disted.size(); ++ix) { }
  }

  void clear() {
    // todo -reset all 'visisted/dark' flags 
    std::vector<LCell*>::iterator i; 
    for (i = disted.begin(); i != disted.end(); ++i) {
      LCell& c = **i;
      c.clear();
    }
  }

  bool blocked(LCell& c, LightMap& light); // Todo: look up 'is map cell blocked' LOI

  void propShadows(LightMap& light);
  void recalcLOS(LightMap& lm); // just a better name for popuLightMap.

  void popuLightMap(LightMap& light);

  void doOctant(LightMap& light, int ax, int ay, int bx, int by);
  CPoint vA, vB;
};

