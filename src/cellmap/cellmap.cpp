#include "stdafx.h"
#include "./cellmap.h"
#include "numutil/myrnd.h"
#include "../Mob.h"
#include <assert.h>
#include "Bag.h"
#include "../MobQueue.h"
#include "DungGen1.h"
#include "Levelize.h"
#include "MobDist.h"

#include "../GrowSeed.h"
#include "../CanSee.h"
#include "../Cuss.h"


void Map::addRandomMob(int levelbase) {  
  const int maxRetries = 75;
  for (int i = 0; i < maxRetries; ++i) {
    CPoint pos(rnd(1, Width), rnd(1, Height));
    assert(legalPos(pos));
    if (!(*this)[pos].creature.empty()) { continue; }
    addRandomMobAtPos(pos,levelbase);
    return; 
  }
}


bool Map::addRandomMobAtPos(CPoint pos, int levelbase) {
  Cell& cell = (*this)[pos];
  if (!cell.creature.empty()) { debstr() << "cell already has mob.\n"; return false; }

  int mlevel = Levelize::suggestLevel(levelbase);  
  Mob* monster = new MonsterMob(mlevel);
  CreatureEnum ctype = MobDist::suggRndMob(mlevel); // Then pick an appropriate creature-type for that mob.
  monster->m_mobType = ctype;

  CL->map.moveMob(*monster, monster->pos);
  CL->mobs.queueMob(monster,1);
  return true;
}


void Map::scatterMobsAtPos(CPoint pos, int n, int level, int radius) {
  for (int i = 0; i < n; ++i) {
    CPoint posA = pos;

    const int maxRetries = 10;
    for (int j = 0; j < maxRetries; ++j) {
      posA.x += rndC(-radius, radius);
      posA.y += rndC(-radius, radius);
      if (!legalPos(posA)) { continue;  }
      if ((*this)[posA].blocked()) { continue;  }
      if (!(*this)[posA].creature.empty()) { continue;  }
      addRandomMobAtPos(posA, level);
      break;
    }
  }
}


void Map::addRandomObj(int level) {
  CPoint pos(rnd(1, Width), rnd(1, Height));
  assert(legalPos(pos));
  addObjAtPos(pos,level);
}


void Map::addObjAtPos(CPoint pos,int levelbase) {
  assert(legalPos(pos));
  Cell& cell = (*this)[pos];
  if (!cell.item.empty()) { debstr() << "cell already has item.\n"; return; }

  const ObjDef& ranDef = Obj::randObjDesc();
  int ilevel = Levelize::suggestLevel(levelbase);  
  Obj* newObj = new Obj(ranDef,ilevel); 

  if (newObj->otype() == OB_Lamp) {
    newObj->itemUnits += rnd(500, 2500);
  }

  if (newObj->otype() ==  OB_WinItem && levelbase < 39) { // You can't find it before level 39..
    newObj->objdef = &Obj::objDesc(OB_Food);
  }
  cell.item.setObj(newObj); // in addObjAtPos.
}


void Map::scatterObjsAtPos(CPoint pos, int n, int level, int radius) {
  for (int i = 0; i < n; ++i) {
    CPoint posA = pos;
    posA.x += rndC(-radius, radius);
    posA.y += rndC(-radius, radius);
    addObjAtPos(posA, level);
  }
}



Map::Map() {
  lightmap.map = this; // necessary init of map ptr.
}


/* maze fixmes, todo:
 - maze generator classifications of cell types are  a mess,
 it's internal 'stage' graph-colouring flags;
which is not even coherent between first 'perfect-maze'
and second 'fill-in deadends' stages.

Further, same classifications end up a mess in the Environment categories,
and similarly, they don't play well with the digging code either.
So far, they've only been 'handled' in the 'iscellblocked' code.
  Further, same messy categories, make the TILE DRAWING a mess (unclear cell types,
and unnecessary even-odd tile effects (because of cell/door grid.) 

Also, the entire thing is a bit slow, in light of wanting to have quick load-cycles;
can partially be solved by smaller maze.
in particular, 'fill-deadends' approach is a bit messy - 
instead of repeated scans, I should probably just fill based on 'found seeds'(?)

Also, treasure in veins should be impl!
*/


void Map::initWorld(int level) {
  if (level == 0) {
    initTown(level);
  } else {
    // alternate between the two:
    level % 2 ? initTunnels2(level) : initTunnels(level);
  }
}

/* done: place monsters on empty squares.
done: place myself on empty.
*/

void Map::initOuterBorders() {
  // husk 1-offset.
  for (int x = 0; x < Width; ++x) {
    CPoint posTop(x, 0); // was: 1
    CPoint posBottom(x, Height - 1);
    Cell& cell1 = (*this)[posTop];    cell1.envir.type = EN_Border;
    Cell& cell2 = (*this)[posBottom]; cell2.envir.type = EN_Border;
  }

  for (int y = 0; y < Height; ++y) { // was : 1 (old hack)
    CPoint posLeft(0, y);
    CPoint posRight(Width-1, y);
    Cell& cell1 = (*this)[posLeft];  cell1.envir.type = EN_Border;
    Cell& cell2 = (*this)[posRight]; cell2.envir.type = EN_Border;    
  }
}




CPoint Map::dirs[4] = { // index of the 4 cardinal directions:
  CPoint(-1, 0),
  CPoint(0, -1),
  CPoint(1, 0),
  CPoint(0, 1)
};

CPoint Map::diag[4] = { // index of the 4 cardinal directions:
  CPoint(-1, -1),
  CPoint( 1, -1),
  CPoint( 1,  1),
  CPoint(-1,  1)
};



int countNeighbours(CPoint p, Laby& laby, MarkEnum mark) {
  // FIXME - don't remove row 1 anymore from map/term.
  int count = 0;
  for (int i = 0; i < 4; ++i) {
  if (laby.grid[ p + Map::dirs[i] ].c == mark) { ++count;  }
  }
  return count;
}

int countOpens(CPoint p, Laby& laby) { return countNeighbours(p, laby, M_Open); }
int countWalls(CPoint p, Laby& laby) { return countNeighbours(p, laby, M_Wall); }

int countDiag(CPoint p, Laby& laby, MarkEnum mark) {
  // FIXME - don't remove row 1 anymore from map/term.
  int count = 0;
  for (int i = 0; i < 4; ++i) {
  if (laby.grid[ p + Map::diag[i] ].c == mark) { ++count;  }
  }
  return count;
}

int countDiagOpens(CPoint p, Laby& laby) { return countDiag(p, laby, M_Open); }
int countDiagWalls(CPoint p, Laby& laby) { return countDiag(p, laby, M_Wall); }

void Map::initTunnels2(int level) {
  GrCanvas laby;
  runSimu(laby, NULL); // CDC* dc);
  /* now do something with laby, transfer it to cellmap. */
  // Create floor/environ.
  for (int x = 0; x < Width; ++x) {
    for (int y = 0; y < Height; ++y) {
      CPoint cur(x, y);
      Cell& cell = (*this)[cur]; 
      if (x == 0 || y == 0 || x == Width - 1 || y == Height - 1) { // The outer border.
        cell.envir.type = EN_Border;
      } else { // Inside-area:
        CPoint p(x, y);

        EnvirEnum etype = EN_Floor;
        bool isWall = laby[p].isMarked();
        switch (isWall) { // laby[p].c%3) {
        case true: etype = EN_Wall;  break; // M_Unvisited
        case false: etype = EN_Floor; break;
        }
        cell.envir.type = etype; // (isWall ? EN_Wall : EN_Floor);

        if (!isWall && oneIn(9)) {
          addObjAtPos(cur, level);
        }

      }
    }
  }
  addStairs();
}



void Map::initTunnels(int level) {

  Laby laby(Map::Width); // 101); // 51); // 101);
  laby.combine(); //  buildAll(); // dc);


  // Create floor/environ.
  for (int x = 0; x < Width; ++x) {
    CellColumn& column = (*this)[x];
    for (int y = 1; y < Height; ++y) {
      Cell& cell = column[y];
      if (x == 0 || y == 1 || x == Width - 1 || y == Height - 1) { // The outer border.
        cell.envir.type = EN_Border;
      } else { // Inside-area:
        CPoint p(x, y);

        EnvirEnum etype = EN_Floor;
        bool isWall = laby.grid[p].isWall(); // oneIn(3);
        switch (laby.grid[p].c) {
        case M_Wall: etype = EN_Wall;  break; // M_Unvisited
        case M_Open: etype = EN_Floor; break;
        case M_Vein: etype = EN_Vein;  break; 
        }

        int openCount = countOpens(p, laby);
        int wallCount = countWalls(p, laby);
        int diagOpen = countDiagOpens(p, laby);
        if (openCount == 2 && wallCount == 2 && laby.grid[p].c == M_Open) {
          if (diagOpen == 2) {
            // etype = EN_DoorOpen;
          } else if (diagOpen == 1) {
            etype = Envir::ranDoor(); // EN_DoorClosed;
          }
        }

        cell.envir.type = etype; // (isWall ? EN_Wall : EN_Floor);

        if (etype == EN_Vein) {
          bool hasThing = oneIn(4);
          if (hasThing) {
            ObjEnum otype = OB_Gold; //  (ObjEnum)rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);
            const ObjDef& goldType = Obj::objDesc(OB_Gold);
            int ilevel = Levelize::suggestLevel(level);
            cell.item.setObj(new Obj(goldType,ilevel)); // In old vein-creation-code.
          }
        }

        if (!isWall) {
          bool hasThing = oneIn(9); // 90); // was: 9);
          if (hasThing) {
            addObjAtPos(p, level);
          }
        }
      }
    }
  } // for x/ create floor.   

  addStairs();
}

void Map::initTown(int level) {
  const char* layoutOld[] = {
    "0", // First row for messages (fixme with viewport offset.)
    "2222222222222222222222222222222222222222222222222",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000022222220000000000000000000000000000002",
    "2000000000022222220000000000000000000000000000002",
    "2000000000022260000000000000000000000000000000002",
    "2000000000022222220000000000000000000000000000002",
    "2000000000022222220000000000000000000000000000002",
    "2000000000000222220000000000000000000000000000002",
    "2000000000000220000000000000000000000000000000002",
    "2000000000000220000000000000000000000000000000002",
    "2000000000000220000000000000000000000000000000002",
    "2000000000000220000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000005000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2000000000000000000000000000000000000000000000002",
    "2222222222222222222222222222222222222222222222222",
    ""
  };

  const char* layout[] = { 
    "&", // First row for messages (fixme with viewport offset.)
    "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&",
    "&...............................................&",
    "&...............................................&",
    "&.......................T.......................&",
    "&...........................GG.............WWWWW&",
    "&........................................WWWWW..&",
    "&..........&&&&&&&...................WWWWWW.....&",
    "&..........&&&&&&&...........T..................1",
    "&..........&&&S.................GG....G.........&",
    "&..........&&&&&&&......WW........WWW...........2",
    "&..........&&&&&&&...........T..WWW....G.G......&",
    "&............&&&&&............T.....T..T........3",
    "&............&&...................G.GG..T.......&",
    "&............&&.................T..T............4",
    "&............&&...................G.....TT......&",
    "&............&&................T................5",
    "&...............................................&",
    "&.................TT............................6",
    "&...............................................&",
    "&................T......>.......................7",
    "&...............................................&",
    "&...................T.....T.....................&",
    "&...............................................&",
    "&...............................................&",
    "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&",
    ""
  };
  for (int y = 0; y < Map::Height && layout[y][0] != '\0'; ++y) {
    const char* row = layout[y];
    for (int x = 0; x < Map::Width && row[x] != '\0'; ++x) {
      CPoint pos(x, y);
      Cell& cell = (*this)[pos];
      //int envirInt = row[x] - '0';
      char envirChar = row[x];
      cell.envir.setType((EnvirEnum)envirChar); // Int);

      if (y == 16) {
        addColDemo(x, y);
      }
    }
  }
  initOuterBorders(); // Ensure we have borders..
}


void Map::addColDemo(int x, int y) {
  CPoint pos(x, y);
  Cell& cell = (*this)[pos];

  const ObjDef& def = Obj::objDesc(OB_Potion);

  SpellEnum spell = (SpellEnum) (x % SP_MaxSpells);
  Obj* newObj = new Obj(def, 1); 
  newObj->effect = spell;
  cell.item.setObj(newObj); // in addColDemo (colour magic identify stuff.)
}


void Map::addStairs() {
  int numStairs = 3;
  for (int i = 0; i < numStairs; ++i) {
    addStair(EN_StairDown);
    addStair(EN_StairUp);
  }
}

void Map::addStair(EnvirEnum type) {
  CPoint stairPos = findFreeEnvir(EN_Floor);
  if (stairPos.x < 0) { return;  } // give up.
  Cell& stairCell = (*this)[stairPos];
  stairCell.envir.type = type;
}

CPoint Map::findFreeEnvir(EnvirEnum type) {
  const int limit = 100;
  for (int i = 0; i < limit; ++i) {
    CPoint cand(rnd(1,Map::Width-1), rnd(1,Map::Height-1));
    if ((*this)[cand].envir.type == type) { return cand; }
  }
  CPoint notFound(-1, 1);
  return notFound;
}



CPoint Map::findNextEnvir(CPoint start, EnvirEnum type) {
  for (CPoint pos = start;;) { // (endless loop)
    ++pos.x;
    if (pos.x >= Map::Width) {
      pos.x = 0;
      ++pos.y;
      if (pos.y >= Map::Height) {
        pos.y = 0;
      }
    }
    Cell& c = (*this)[pos];
    if (c.envir.type == type) { return pos; }
    if (pos == start) { 
      CPoint notFound(-1, -1);
      return notFound;
    }
  }

  return CPoint(-1, -1);
}



CPoint Map::findNextMob(CPoint start, CreatureEnum ctype) {
  for (CPoint pos = start;;) { // (endless loop)
    ++pos.x;
    if (pos.x >= Map::Width) {
      pos.x = 0;
      ++pos.y;
      if (pos.y >= Map::Height) {
        pos.y = 0;
      }
    }
    Cell& c = (*this)[pos];
    if (c.creature.type() == ctype) { return pos; }
    if (pos == start) { 
      CPoint notFound(-1, -1);
      return notFound;
    }
  }

  return CPoint(-1, -1);
}


CPoint Map::key2dir(char nChar) {
  int dx = 0, dy = 0;
  // determine movement:
  switch (nChar) { case VK_RIGHT:  case 'L': case 'U': case 'N': dx = 1; }
  switch (nChar) { case VK_LEFT:   case 'H': case 'Y': case 'B': dx = -1; }
  switch (nChar) { case VK_DOWN:   case 'J': case 'B': case 'N': dy = 1; }
  switch (nChar) { case VK_UP:     case 'K': case 'Y': case 'U': dy = -1; }
  CPoint dir(dx, dy);
  return dir;
}




void Map::moveMob(class Mob& m, CPoint newpos) {
  (*this)[m.pos].creature.clearMob();
  m.pos = newpos;
  (*this)[m.pos].creature.setMob(&m);

  if (m.isPlayer()) {
    Viewport::vp.adjust(m.pos);

    bool bChangedPos = !!(lightmap.map_offset != newpos);
    if (bChangedPos) {
      lightmap.map_offset = newpos;
      LOS::los.recalcLOS(lightmap);
      // fixme/Todo: is some sort of invalidategfx.. necessary?
    }
  }
}




void Map::addObj(class Obj& o, CPoint pos) {
  (*this)[pos].item.setObj(&o);
}



Cell* Map::cell(CPoint p) { 
  if (p.x < 0 || p.x >= Map::Width || p.y < 0 || p.y >= Map::Height) {
    return NULL;
  }
  assert(p.x >= 0);
  assert(p.y >= 0);
  assert(p.x < Map::Width);
  assert(p.y < Map::Height);
  return &cellColumns[p.x][p.y];  
}


Cell& Map::operator [] (CPoint p) { 
  if (p.x < 0 || p.x >= Map::Width || p.y < 0 || p.y >= Map::Height) {
    int badThingsHappen = 42;
  }
  assert(p.x >= 0);
  assert(p.y >= 0);
  assert(p.x < Map::Width);
  assert(p.y < Map::Height);
  return cellColumns[p.x][p.y];  
}


CellColumn& Map::operator [] (int x) { 
  static CellColumn noneColumn;
  if (x < 0 || x >= Map::Width) { return noneColumn; }

  assert(x >= 0);
  assert(x < Map::Width);
  return cellColumns[x];  
}


Cell& CellColumn::operator [] (int y) { 
  static Cell noneCell;
  if (y < 0 || y >= Map::Height) { return noneCell; }

  assert(y >= 0);
  assert(y < CellColumn::Height);
  return cells[y];  
}




bool Map::legalPos(CPoint pos) {
  if (pos.x < 0 || pos.y < 0) { return false; }
  if (pos.x >= Map::Width || pos.y >= Map::Height) { return false;  }
  return true;
}





Viewport Viewport::vp;

Viewport::Viewport() {
  const int sweetspotWidth  = (SweetspotPct * Width / 100);
  const int sweetspotHeight = (SweetspotPct * Height / 100);
  sweetspotArea.top = sweetspotHeight;
  sweetspotArea.left = sweetspotWidth;
  sweetspotArea.right = Width - sweetspotWidth;
  sweetspotArea.bottom = Height - sweetspotHeight;
}



bool Viewport::adjust(CPoint wpos) { // True if adjust happens.
  VPoint vp; 
  vp.p = w2v(wpos); //wpos - offset;
  if (sweetspotArea.PtInRect(vp.p)) { return false; } // No adjustment necessary.

  // Move offset, so wpos is in center of screen.

  if (vp.p.x < sweetspotArea.left || vp.p.x > sweetspotArea.right) {
    int halfVPWidth = (Width / 2);
    offset.x = wpos.x - halfVPWidth;

    if (wpos.x < halfVPWidth) { offset.x = 0; } // If we are close to left edge, lock it.
    if (wpos.x > (Map::Width - halfVPWidth)) { offset.x = (Map::Width - Width); } // If we are close to right edge, lock it.
  }

  if (vp.p.y < sweetspotArea.top || vp.p.y > sweetspotArea.bottom) {
    int halfVPHeight = (Height / 2);
    offset.y = wpos.y - halfVPHeight;

    if (wpos.y < halfVPHeight) { offset.y = 0; } // If we are close to left edge, lock it.
    if (wpos.y > (Map::Height - halfVPHeight)) { offset.y = (Map::Height - Height); } // If we are close to bottom edge, lock it.
  }

  /* If we are 'cornered', we don't really want center; 
  instead we want to fill viewport.
  How can I describe this more completely.. 
  ( a correct description will probably illustrate how to impl.)

  It's something about being close to corners..
  If the distance to a corner is smaller than.. "half-screen-height?", 
  we should probably just stay there?
  */

  Cuss::invalidate();
  return true; // We adjusted..
}




bool Map::persist(Persist& p) {
  int objCount = 0; // (count obj's, to aid later output of obj-list.)
  // First, output floor-cells:

  for (int y = 0; y < Height; ++y) {
    for (int x = 0; x < Width; ++x) {
      CPoint pos(x, y);
      Cell& cell = (*this)[pos];
      cell.persist(p);
      if (!cell.item.empty()) {
        ++objCount;
      }
    }
    if (p.bOut) {
      p.os << "\n";
    }
  }

  p.transfer(objCount, "objCount");

  if (p.bOut) {
    // Output objects:
    for (int x = 0; x < Width; ++x) {
      CellColumn& column = (*this)[x];
      for (int y = 1; y < Height; ++y) {
        Cell& cell = column[y];
        if (!cell.item.empty()) {
          CPoint pos(x, y);
          cell.item.o->persist(p,pos);
        }
      }
    }
  } else { // Use objCount:
    for (int i = 0; i < objCount; ++i) {
      transferObj(p); // NB, WILL lead to Obj::persist eventually.
    }
  }

  return true;
}






bool Map::transferObj(Persist& p) { // Only works for obj IN, to map:
  const ObjDef& dummy = Obj::objDesc(OB_None);
  Obj* o = new Obj(dummy,1);
  CPoint pos;
  o->persist(p, pos);
  Cell& cell = (*this)[pos];
  cell.item.setObj(o); // In transferObj (persistence helper.)
  return true;
}


bool Map::canSee(CPoint a, CPoint b, bool onlyEnvir) {
  return CanSee::canSee(a, b, *this, onlyEnvir);
}