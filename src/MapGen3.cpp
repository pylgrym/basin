#include "stdafx.h"
#include "MapGen3.h"

#include "cellmap/cellmap.h"
#include "./GrowSeed.h"


MapGen3::MapGen3()
{
}


MapGen3::~MapGen3()
{
}



void Map::initTunnels2(int level) {
  GrCanvas laby(Width,Height); // FIXME - same problem with width/height!

  runSimu(laby, NULL); // CDC* dc);
  /* now do something with laby, transfer it to cellmap. */
  // Create floor/environ.
  for (int x = 0; x < Width; ++x) {
    for (int y = 0; y < Height; ++y) {
      CPoint cur(x, y);
      Cell& cell = (*this)[cur];
      if (x == 0 || y == 0 || x == Width - 1 || y == Height - 1) { // The outer border.
        cell.envir.type = EN_Border;
      }
      else { // Inside-area:
        CPoint p(x, y);

        EnvirEnum etype = EN_Floor;
        bool isWall = laby[p].isMarked();
        switch (isWall) { // laby[p].c%3) {
        case true: etype = EN_Wall;  break; // M_Unvisited
        case false: etype = EN_Floor; break;
        }
        cell.envir.type = etype; // (isWall ? EN_Wall : EN_Floor);

        if (!isWall && rnd::oneIn(9)) {
          addObjAtPos(cur, level);
        }

      }
    }
  }
  addStairs(Map::Width, Map::Height);
}
