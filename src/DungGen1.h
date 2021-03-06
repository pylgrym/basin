#pragma once

#include <vector>
#include <assert.h>
#include "numutil/myrnd.h"

class DungGen1
{
public:
  DungGen1();
  ~DungGen1();
};



enum MarkEnum {
  
  M_Wall = 1,
  M_Open = 2,
  M_Vein = 6
};



class Mark {
public:
  MarkEnum c;
  int count;
  COLORREF color;
  Mark() {
    c = M_Wall;
    count = -1;
    color = RGB(192, 128,64);
  }

  bool visited() const { return c != M_Wall; } 
  bool blocked() const { return c == M_Wall; } 
  void occupyMark(MarkEnum id) { c = id; }

  bool isWall() { 
    return (c == M_Wall); 
  }

  void makeVein() { c = M_Vein;  }
  static bool veinCat(MarkEnum m); 
};


//int rnd(int n); // { return rand() % n; }
//int rnd(int n1, int n2); // { return n1 + rand() % (n2 - n1); }



struct Pool : public std::vector < CPoint > {
  void Push(CPoint p) { push_back(p); } 
  bool Empty() const { return empty(); }
  int Size() { return size(); }
  CPoint Item(int ix) { return operator [] (ix); }
};




struct Grid {
  Grid(int sideW_, int sideH_) :SideW(sideW_), SideH(sideH_) {
    cells.resize(SideW);
    for (int i = 0; i < SideW; ++i) {
      cells[i].resize(SideH);
    }
  }

  std::vector< std::vector< Mark > > cells;
  const int SideW, SideH;

  Mark& operator [] (CPoint p) { return cell(p); }

  Mark& cell(CPoint p) {
    if (p.x >= SideW) {
      int q = 19;
    }
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < SideW);
    assert(p.y < SideH);
    return cells[p.x][p.y];
  }
};

struct Laby {
  Laby(int sideW_, int sideH_) :SSideW(sideW_), SSideH(sideH_),grid(sideW_, sideH_) {}

  const int SSideW, SSideH;

  Grid grid;

  void buildMaze(); 
  void thinnedMaze(int fillGrade);
  void combine();
  void addVeins(Laby& veins);

  bool veinTrigger(CPoint self) ;


  bool isVisited(CPoint p) {
    if (p.x < 1 || p.y < 1 || p.x >= SSideW - 1 || p.y >= SSideH - 1) {
      return true;
    }
    return grid[p].visited(); 
  }

  bool isBlocked(CPoint p) { // Cell-wise, reverse of original
    if (p.x < 1 || p.y < 1 || p.x >= SSideW - 1 || p.y >= SSideH - 1) {
      return true;
    }
    return grid[p].blocked(); 
  }


  void draw(CDC& dc) {
    dc.SetBkMode(TRANSPARENT);
    for (int x = 0; x < SSideW; ++x) {
      for (int y = 0; y < SSideH; ++y) {
        CPoint p(x, y);
        drawCell(p, grid[p], dc);
      }
    }
  }

  void drawCell(CPoint p, Mark& m, CDC& dc);

  void addRooms() {
    for (int i = 0; i < 50; ++i) {
      addRoom();
    }
  }

  void addRoom() {
    CSize s(rnd::Rnd(2, 4) * 2, rnd::Rnd(3, 7) * 2);
    if (rnd::Rnd(0, 2)) {
      int tmp = s.cx;
      s.cx = s.cy; s.cy = tmp;
    }
    CPoint ul(1 + (rnd::Rnd(1, SSideW - s.cx - 1) / 2) * 2, 1 + (rnd::Rnd(1, SSideH - s.cy - 1) / 2) * 2);
    CRect r(ul, s);
    fillRoom(r);
  }

  void fillRoom(CRect r) {
    extern COLORREF rndColor();
    COLORREF randCol = rndColor();

    for (int x = r.left; x <= r.right; ++x) {
      for (int y = r.top; y <= r.bottom; ++y) {
        CPoint p(x, y);
        Mark& m = grid[p];
        m.c = M_Open; 
        m.color = randCol;
      }
    }
  }

  void fillDeadends() {
    for (int x = 1; x < SSideW - 1; x += 2) {
      for (int y = 1; y < SSideH - 1; y += 2) {
        CPoint p(x, y);
        fillIfDeadend(p);
      }
    }
  }

  void fillIfDeadend(CPoint p) {
    std::vector<CPoint> doors;
    int count = getDoors(p, doors);
    grid[p].count = count;
    if (count == 1) {
      CPoint dir = doors[0];
      CPoint hall = p + dir;
      CPoint room = p;
      grid[hall].c = M_Wall;
      grid[room].c = M_Wall;
    }
  }

  int getDoors(CPoint p, std::vector<CPoint>& doors);

};
