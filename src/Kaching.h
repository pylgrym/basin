#pragma once

// #include "theMark.h"
#include <vector>
#include "numutil/myrnd.h"

// int rnd(int n); // { return rand() % n; }
//int rnd(int n1, int n2); // { return n1 + rand() % (n2 - n1); }



struct Blob {
  enum Consts {
    Side = 50, // 130, 
    Count = 40 // 200
  };
  CRect r;
  bool deadSides[4];
  int id;
  COLORREF color;
  CPoint door;

  Blob(int id_) :id(id_) {
    CPoint p((rnd::Rnd(Side) / 3) * 3, (rnd::Rnd(Side) / 3) * 3);

    r = CRect(p, CSize(0, 0)); // 1, 1));
    for (int i = 0; i < 4; ++i) {
      deadSides[i] = false;
    }
    const int fac = 120;
    color = RGB(rnd::Rnd(fac), rnd::Rnd(fac), rnd::Rnd(fac));
  }

  void addDoor(CDC* dc, struct Blobs& blobs);
  void clearInner(CDC* dc, struct Blobs& blobs);

  void run(CDC* dc, struct Blobs& queue);
};


class MarkB {
public:
  int c;
  // int count;

  MarkB() {
    c = 0; // M_Unvisited;
    // count = -1;
  }

  bool visited() const { return c != 0; }

  //bool blocked() const { return c == M_Wall || c == M_Wall_H || c == M_Unvisited; }
  void occupyMark(int id) { c = id; }
};

struct Blobs {
  std::vector<Blob*> blobs;

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
  MarkB marks[Blob::Side][Blob::Side];
  bool isLegal(CPoint a);
  void occupySide(CPoint a, CPoint b, CPoint dir, Blob& z, CDC* dc);

  static void plotBox(CPoint a, CPoint b, COLORREF color, CDC* dc);

};

