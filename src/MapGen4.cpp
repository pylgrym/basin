#include "stdafx.h"
#include "MapGen4.h"

#include "cellmap/cellmap.h"
#include "BungBrownian.h"

MapGen4::MapGen4()
{
}


MapGen4::~MapGen4()
{
}


void Map::initTunnelsBrown(int level) {
  Brown brown(Width2, Height2,NULL);
  brown.run();

  // GrCanvas laby(Width2, Height2); // FIXME - same problem with width/height!
  // runSimu(laby, NULL); // CDC* dc);

  /* now do something with laby, transfer it to cellmap. */
  // Create floor/environ.
  for (int x = 0; x < Width2; ++x) {
    for (int y = 0; y < Height2; ++y) {
      CPoint cur(x, y);
      Cell& cell = (*this)[cur];
      if (x == 0 || y == 0 || x == Width2 - 1 || y == Height2 - 1) { // The outer border.
        cell.envir.type = EN_Border;
      }
      else { // Inside-area:
        CPoint p(x, y);

        EnvirEnum etype = EN_Floor;
        bool isWall = true;

        int val = brown.cell(p).c;
        switch (val) {
        case BrCell::BE_Empty: etype = EN_Border; break;//   EN_DoorCullis; break;
        case BrCell::BE_Floor: etype = EN_Floor; isWall = false; break;
        case BrCell::BE_Hall: etype = EN_Green; isWall = false; break;
        case BrCell::BE_Room: etype = EN_FloorRed; break;
        case BrCell::BE_Wall: etype = EN_Wall; break;
        }
        cell.envir.type = etype; 

        if (!isWall && rnd::oneIn(9)) {
          addObjAtPos(cur, level);
        }

      }
    }
  }
  addStairs(Width2, Height2);
}
