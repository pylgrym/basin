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
  Mark() {
    c = M_Wall;
    count = -1;
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
  Grid(int side_) :Side(side_) {
    cells.resize(Side);
    for (int i = 0; i < Side; ++i) {
      cells[i].resize(Side);
    }
  }

  std::vector< std::vector< Mark > > cells;
  int Side;

  Mark& operator [] (CPoint p) { return cell(p); }

  Mark& cell(CPoint p) {
    if (p.x >= Side) {
      int q = 19;
    }
    assert(p.x >= 0);
    assert(p.y >= 0);
    assert(p.x < Side);
    assert(p.y < Side);
    return cells[p.x][p.y];
  }
};

struct Laby {
  Laby(int side_) :Side(side_), grid(side_) {}

  const int Side;

  Grid grid;

  void buildMaze(); 
  void thinnedMaze(int fillGrade);
  void combine();
  void addVeins(Laby& veins);

  bool veinTrigger(CPoint self) ;


  bool isVisited(CPoint p) {
    if (p.x < 1 || p.y < 1 || p.x >= Side - 1 || p.y >= Side - 1) {
      return true;
    }
    return grid[p].visited(); 
  }

  bool isBlocked(CPoint p) { // Cell-wise, reverse of original
    if (p.x < 1 || p.y < 1 || p.x >= Side - 1 || p.y >= Side - 1) {
      return true;
    }
    return grid[p].blocked(); 
  }


  void draw(CDC& dc) {
    dc.SetBkMode(TRANSPARENT);
    for (int x = 0; x < Side; ++x) {
      for (int y = 0; y < Side; ++y) {
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
    CSize s(Rnd::rnd(2, 4) * 2, Rnd::rnd(3, 7) * 2);
    if (Rnd::rnd(0, 2)) {
      int tmp = s.cx;
      s.cx = s.cy; s.cy = tmp;
    }
    CPoint ul(1 + (Rnd::rnd(1, Side - s.cx - 1) / 2) * 2, 1 + (Rnd::rnd(1, Side - s.cy - 1) / 2) * 2);
    CRect r(ul, s);
    fillRoom(r);
  }

  void fillRoom(CRect r) {
    for (int x = r.left; x <= r.right; ++x) {
      for (int y = r.top; y <= r.bottom; ++y) {
        CPoint p(x, y);
        grid[p].c = M_Open; 
      }
    }
  }

  void fillDeadends() {
    for (int x = 1; x < Side - 1; x += 2) {
      for (int y = 1; y < Side - 1; y += 2) {
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
