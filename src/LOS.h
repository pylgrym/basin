#pragma once

#include <vector>
#include <set>
#include <algorithm>
#include <assert.h>

#include "Bresenham.h"

extern bool operator < (CPoint a, CPoint b);

class LCell : public CPoint {
public:
  LCell() { ix = 0; distsq = 0; dark = false; visited = false; }
  int ix;
  // int x, y, ix;

  int distsq;
  int calcdistsq() const { return x*x + y*y; }

  bool dark;
  bool visited;
  // These are only populated in some cases.
  std::set<CPoint> behinds; // all points immediately behind me.
  std::vector<CPoint> fronts; // all points immediately in front of me.
};

class NearOrder {
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
  LOS();
  ~LOS();



  void test() {
    /* fixme: establish what 'visited' really means
     - which invariants we are trying to establish,
     and what the various processing steps are supposed to do.
    
     - which kinds of 'visits' do we allow from several directions?
    */

    // Iterate through cells, furthest first.
    std::vector<LCell*>::reverse_iterator i; 
    for (i = disted.rbegin(); i != disted.rend(); ++i) {
      LCell& c = **i;
      if (c.visited) { continue; }
      c.visited = true;

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
};

