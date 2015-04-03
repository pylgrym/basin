#pragma once

#include <assert.h>
#include <vector>

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
  const int SideW;
  const int SideH;

  enum { 
    MaxPool = 150, 
    // SideW,SideH = 101 
  };

  GrCanvas(int w, int h) :SideW(w), SideH(h) {
    cells_i.resize(SideW);
    for (int column = 0; column < SideW; ++column) {
      cells_i[column].resize(SideH);
    }
  }

  std::vector< std::vector < ACell > > cells_i;
  // ACell cells[SideW][SideH];

  ACell& cells(CPoint p) {
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < SideW);
    assert(p.y < SideH);
    return cells_i[p.x][p.y];
  }


  ACell& operator [] (CPoint p) {
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < SideW);
    assert(p.y < SideH);
    return cells(p); // [p.x][p.y];
  }

  bool legalPos(CPoint p) const {
    return (p.x >= 0 && p.y >= 0 && p.x < SideW && p.y < SideH);
  }

};


extern bool runSimu(GrCanvas& cells, CDC* dc);

class GrowSeed
{
public:
  GrowSeed();
  ~GrowSeed();
};


