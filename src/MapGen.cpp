#include "stdafx.h"
#include "MapGen.h"

#include "cellmap/cellmap.h"

#include "Kaching.h"

#include <map>




MapGen::MapGen()
{
}


MapGen::~MapGen()
{
}


struct RoomColors {
  std::map< int, EnvirEnum > floors;
  EnvirEnum floorForRoom(int id) {
    if (floors.find(id) == floors.end()) {
      floors[id] = Envir::ranFloor();
    }
    return floors[id];
  }
};



void Map::initKachingBlob(int level) {
  /* fixme, this is wrong: The 3 map generator things should go in a separate file,
  not in the general cellmap.cpp!
  */
  RoomColors floorTypes;

  // Blob blob(0); // JG: I'm not sure we really ended up using the blob-thing for anything
  // (it's the queue we are interested in?)

  Blobs queue(Width, Height);

  queue.run(NULL); // , queue); // &dc);
  for (int x = 0; x < queue.SideW; ++x) {
    for (int y = 0; y < queue.SideH; ++y) {
      CPoint p(x, y);
      MarkB& m = queue.marks(p);
      EnvirEnum etype = EN_Floor;
      const int type = m.c;
      if (type == 0) { // Unvisited: ("outside street")
        etype = EN_Floor;
      }
      else if (type == -1000) {
        etype = Envir::ranDoor();
      }
      else if (type < 0) { // Get floors from a std::map<idcolor, randchosenfloor>
        etype = floorTypes.floorForRoom(m.c);
        if (rnd::oneIn(9)) {
          addObjAtPos(p, level);
        }
      }
      else if (type > 0) { // wall.
        etype = Envir::ranWall();
      }

      Cell& cell = (*this)[p];
      cell.envir.type = etype;
    }
  }
  // Fix Borders:
  initOuterBorders();
  addStairs(queue.SideW, queue.SideH);
}






