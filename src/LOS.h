#pragma once

#include <vector>
#include <algorithm>

class LCell {
public:
  int x, y, ix;
  int distsq;
  int calcdistsq() const { return x*x + y*y; }
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

  void initDisted() {
    int ix = 0;
    for (int x = 0; x < Side; ++x) {
      for (int y = 0; y < x; ++y, ix++) {
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

  void test();
};

