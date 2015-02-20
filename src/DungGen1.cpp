#include "stdafx.h"
#include "DungGen1.h"


DungGen1::DungGen1() {}
DungGen1::~DungGen1(){}




CPoint scale(CPoint p) {
  int sc = 5;
  return CPoint(p.x*sc, p.y*sc);
}




CPoint dirs[4] = { // index of the 4 cardinal directions:
  // JG: I don't set this to go 2 instead of 1: - because I can always multiply a '1 vector' to get a 2-vector.
  CPoint(-1, 0),
  CPoint(0, -1),
  CPoint(1, 0),
  CPoint(0, 1)
};





void Laby::buildMaze() { // CDC& dc) {
  Pool pool;  // pool of active cells

  for (int i = 0; i < 1; ++i) {
    CPoint seed(rnd(1, Side / 2) * 2 + 1, rnd(1, Side / 2) * 2 + 1); // Side / 2, Side / 2);
    if (isVisited(seed)) { continue; }
    pool.Push(seed); // add a seed cell to the pool.    
    grid[seed].c = M_Visited; // Now taken.
  }

  while (!pool.Empty()) {
    int ix = pool.size() - 1; // rnd(pool.Size());
    CPoint p = pool.Item(ix); // pick a 'random' cell from pool.

    // check open dirs:
    CPoint options[4];
    int options_count = 0;

    for (int d = 0; d < 4; ++d) {
      CPoint nei = p + dirs[d] + dirs[d];
      if (!isVisited(nei)) {
        options[options_count++] = dirs[d];
      }
    } // for dirs.

    if (options_count == 0) {
      if (pool.Size() > 1) { // If more than one left, swap dead with last
        pool[ix] = pool.back(); // pool.Pop();
      }
      pool.resize(pool.Size() - 1);
      continue;
    }

    int choice_ix = rnd(options_count);
    CPoint dir = options[choice_ix];
    CPoint choice = p + dir + dir;

    pool.Push(choice);
    // mark in cells.
    grid[p + dir].c = M_Open; // Now taken. (the doorway.)        
    grid[choice].c = M_Visited; // Now taken. (the next room.)

    // Draw from p to choice.
    // dc.MoveTo(scale(p));
    //dc.LineTo(scale(choice));

  } // while pool.
}



void Laby::thinnedMaze(int fillGrade) {
  // perfect maze, followed by filling tunnels up.
  buildMaze();
  for (int maria = 1; maria < fillGrade; ++maria) {
    fillDeadends();
  }
}

void Laby::combine() {
  int mainThin = 5, veinThin = 5;
  bool proper = false; // true;
  if (proper) {
    mainThin = 20; veinThin = 120;

  }
  thinnedMaze(mainThin); // 5); // 20); // 20 here, 120 there, is good.. ( but slow.)

  bool doVeins = false;
  if (doVeins) { // Build an extra maze, to use as 'mineral veins':
    Laby veins(Side); veins.thinnedMaze(veinThin); // 5); // 120);
    addVeins(veins);
  }

  addRooms();

}

void Laby::addVeins(Laby& veins) {
  for (int x = 1; x < Side-1; ++x) {
    for (int y = 1; y < Side-1; ++y) {
      CPoint p(x, y);
      if (veins.veinTrigger(p) && grid[p].isWall() ) { 
        grid[p].makeVein();
      }
    }
  }
}

bool Laby::veinTrigger(CPoint self)   {
  if (Mark::veinCat(grid[self].c)) { return true;  }
  return false; 

  // I don't want thick veins.
  for (int d = 0; d < 4; ++d) {
    CPoint nei = self + dirs[d];
    if (Mark::veinCat(grid[nei].c)) {
      return true;
    }
  }

  return false; 
}


bool Mark::veinCat(MarkEnum m)  {
  if (m == M_OpenB || m == M_Open) { return true; }
  return false;
}


void Laby::drawCell(CPoint p, Mark& m, CDC& dc) {
  CSize s(5, 5); // 15, 15); // 5, 5);
  CPoint z(p.x*s.cx, p.y*s.cy);
  CRect r(z, s);
  COLORREF c = RGB(0, 255, 0);
  switch (m.c) {
  case M_Unvisited: c = RGB(0, 0, 128); break; // wall

  case M_Open: c = RGB(170, 170, 170); break; // open hallway.
  case M_Visited: c = RGB(160, 130, 80); break; // room.

  case M_Wall: c = RGB(20, 20, 170); // 120, 120, 120); break; // filled-in room.
  case M_Wall_H: c = RGB(0, 0, 255); break;   // filled-in hall.

  case M_Vein: c = RGB(255, 20, 0); break; // mineral wall/rock.
  }
  CBrush brush(c); // m.c == 1 ? RGB(199, 199, 199) : RGB(0, 0, 64));
  dc.FillRect(&r, &brush);

  return;
  if (m.count < 0) { return; }
  CString a; a.Format(L"%d", m.count);
  int fmt = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
  dc.DrawText(a, &r, fmt);
}



int Laby::getDoors(CPoint p, std::vector<CPoint>& doors) {
  // check open dirs:
  for (int d = 0; d < 4; ++d) {
    CPoint dir = dirs[d];
    CPoint nei = p + dir;
    if (!isBlocked(nei)) {
      doors.push_back(dir);
    }
  } // for dirs.
  return doors.size();
}


