#pragma once

#include <assert.h>

class ACell { 
public:
  int c;  
  enum { ClearVal = 0 };
  ACell() { clear(); } 

  void clear() { c = ClearVal; }
  void mark(int seedID) { c = seedID; } 
  bool isClear() { return c == ClearVal; }
  bool isMarked() { return c != ClearVal; }
};


class GrCanvas {
public:
  enum { 
    MaxPool = 150, 
    Side = 101 
  };

  ACell cells[Side][Side];

  ACell& operator [] (CPoint p) {
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < Side);
    assert(p.y < Side);
    return cells[p.x][p.y];
  }

  bool legalPos(CPoint p) const {
    return (p.x >= 0 && p.y >= 0 && p.x < Side && p.y < Side);
  }

};


extern bool runSimu(GrCanvas& cells, CDC* dc);

class GrowSeed
{
public:
  GrowSeed();
  ~GrowSeed();
};


