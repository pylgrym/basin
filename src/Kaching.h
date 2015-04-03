#pragma once

#include <vector>
#include "numutil/myrnd.h"
#include <assert.h>


struct MarkB {
  int c;

  MarkB() { c = 0;  }  // M_Unvisited;
  bool visited() const { return c != 0; }
  void occupyMark(int id) { c = id; }
};


struct Blob {

  CRect r;
  bool deadSides[4];
  int id;
  COLORREF color;
  CPoint door;

  Blob(int id_, struct Blobs& blobs);

  void addDoor(CDC* dc, struct Blobs& blobs);
  void clearInner(CDC* dc, struct Blobs& blobs);
};



struct Blobs {
  Blobs(int SideW_, int SideH_):SideW(SideW_),SideH(SideH_) 
  {
    marks_i.resize(SideW);
    for (int column = 0; column < SideW; ++column) {
      marks_i[column].resize(SideH);
    }
  }

  // MarkB marks[Blob::SideW][Blob::SideH];
  std::vector< std::vector <MarkB> > marks_i;
  MarkB& marks(CPoint p) {
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < SideW);
    assert(p.y < SideH);
    return marks_i[p.x][p.y];
  }

  int SideW, SideH;

  enum Consts {
    // SideW = 50, // 130, 
    // SideH = 50,
    Count = 40 // 200
  };

  std::vector<Blob*> blobs;

  void run(CDC* dc); // , struct Blobs& queue);

  void remove(int ix) {
    if (blobs.size() >= 2) {
      // As long as we have at least N items, give the old spot to last item:
      Blob* last = blobs[blobs.size() - 1];
      blobs[ix] = last;
    }
    blobs.resize(blobs.size() - 1);
  }

  bool growSomeSide(Blob& b, CDC* dc);
  void exploreSide(int side, Blob& b, CDC* dc);
  bool sideFree(CPoint a, CPoint b, CPoint dir, CPoint nei);
  bool occupied(CPoint a);
  bool isLegal(CPoint a);
  void occupySide(CPoint a, CPoint b, CPoint dir, Blob& z, CDC* dc);

  static void plotBox(CPoint a, CPoint b, COLORREF color, CDC* dc);

};

