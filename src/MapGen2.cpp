#include "stdafx.h"
#include "MapGen2.h"


#include "cellmap/cellmap.h"
#include "DungGen1.h"
#include "Levelize.h" // mapgen2

MapGen2::MapGen2()
{
}


MapGen2::~MapGen2()
{
}




CPoint Map::dirs[4] = { // index of the 4 cardinal directions:
  CPoint(-1, 0),
  CPoint(0, -1),
  CPoint(1, 0),
  CPoint(0, 1)
};

CPoint Map::diag[4] = { // index of the 4 cardinal directions:
  CPoint(-1, -1),
  CPoint(1, -1),
  CPoint(1, 1),
  CPoint(-1, 1)
};



int countNeighbours(CPoint p, Laby& laby, MarkEnum mark) {
  // FIXME - don't remove row 1 anymore from map/term.
  int count = 0;
  for (int i = 0; i < 4; ++i) {
    if (laby.grid[p + Map::dirs[i]].c == mark) { ++count; }
  }
  return count;
}

int countOpens(CPoint p, Laby& laby) { return countNeighbours(p, laby, M_Open); }
int countWalls(CPoint p, Laby& laby) { return countNeighbours(p, laby, M_Wall); }

int countDiag(CPoint p, Laby& laby, MarkEnum mark) {
  // FIXME - don't remove row 1 anymore from map/term.
  int count = 0;
  for (int i = 0; i < 4; ++i) {
    if (laby.grid[p + Map::diag[i]].c == mark) { ++count; }
  }
  return count;
}

int countDiagOpens(CPoint p, Laby& laby) { return countDiag(p, laby, M_Open); }
int countDiagWalls(CPoint p, Laby& laby) { return countDiag(p, laby, M_Wall); }

void Map::initTunnels(int level) {

  // FIXME/NB! - 'map' is different width+height, whereas this algo is same width-height!

  Laby laby(Width2, Height2); // Width); // 101); // 51); // 101);
  laby.combine(); //  buildAll(); // dc);


  // Create floor/environ.
  for (int x = 0; x < Width; ++x) {
    //CellColumn& column = (*this)[x];
    for (int y = 0; y < Height; ++y) {
      CPoint p(x, y);
      Cell& cell = (*this)[p]; // column[y];
      if (x == 0 || y == 0 || x == Width - 1 || y == Height - 1) { // The outer border.
        cell.envir.type = EN_Border;
      }
      else if (x < Map::Width) { // Height) { // Inside-area: // NB, danger here - width/height!
        CPoint p(x, y);
        Mark& mark = laby.grid[p];

        COLORREF ecolor = colorNone;
        EnvirEnum etype = EN_Floor;
        bool isWall = laby.grid[p].isWall();
        switch (laby.grid[p].c) {
        case M_Vein: etype = EN_Vein;  break;
        case M_Wall: etype = EN_Wall;  break;
        case M_Open: etype = EN_Floor;
          ecolor = mark.color;
          break;
        }

        int openCount = countOpens(p, laby);
        int wallCount = countWalls(p, laby);
        int diagOpen = countDiagOpens(p, laby);
        if (openCount == 2 && wallCount == 2 && laby.grid[p].c == M_Open) {
          if (diagOpen == 2) {
            // etype = EN_DoorOpen;
          }
          else if (diagOpen == 1) {
            etype = Envir::ranDoor(); // EN_DoorClosed;
          }
        }

        cell.envir.type = etype; // (isWall ? EN_Wall : EN_Floor);
        cell.envir.ecolor = ecolor;

        if (etype == EN_Vein) {
          bool hasThing = rnd::oneIn(4);
          if (hasThing) {
            ObjEnum otype = OB_Gold; //  (ObjEnum)rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);
            const ObjDef& goldType = Obj::objDesc(OB_Gold);
            int ilevel = Levelize::suggestLevel(level);
            cell.item.setObj(new Obj(goldType, ilevel)); // In old vein-creation-code.
          }
        }

        if (!isWall) {
          bool hasThing = rnd::oneIn(9); // 90); // was: 9);
          if (hasThing) {
            addObjAtPos(p, level);
          }
        }
      }
    }
  } // for x/ create floor.   

  addStairs(Width2, Height2);
}
