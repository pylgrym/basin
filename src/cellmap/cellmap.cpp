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


void Map::resizeMap() {
  cells_i.resize(Width2);
  for (int column = 0; column < Width2; ++column) {
    cells_i[column].resize(Height2);
  }
}

Map::Map(int w, int h):Width2(w), Height2(h) {
  resizeMap();

  lightmap.map = this; // necessary init of map ptr.
}

// CellColumn& Map::cellColumns(int colIx) {  return cellColumns_i[colIx]; }





Cell* Map::cell(CPoint p) { 
  if (p.x < 0 || p.x >= Width2 || p.y < 0 || p.y >= Height2) {
    return NULL;
  }
  assert(p.x >= 0);
  assert(p.y >= 0);
  assert(p.x < Width2);
  assert(p.y < Height2);
  return &cells_i[p.x][p.y];// cellColumns(p.x)[p.y];
}


Cell& Map::operator [] (CPoint p) { 
  if (p.x < 0 || p.x >= Width2 || p.y < 0 || p.y >= Height2) {
    int badThingsHappen = 42;
  }
  assert(p.x >= 0);
  assert(p.y >= 0);

  if (!(p.x < Map::Width2)) {
    DebugBreak();
  }
  assert(p.x < Width2);

  if (!(p.y < Height2)) {
    DebugBreak();
  }
  assert(p.y < Height2);

  return cells_i[p.x][p.y]; // cellColumns(p.x)[p.y];
}


/// CellColumn& Map::operator [] (int x) { 
///   static CellColumn noneColumn;
///   if (x < 0 || x >= Map::Width) { return noneColumn; }
/// 
///   assert(x >= 0);
///   assert(x < Map::Width);
///   return cellColumns(x);  
/// }


/// Cell& CellColumn::operator [] (int y) { 
///   static Cell noneCell;
///   if (y < 0 || y >= Map::Height) { return noneCell; }
/// 
///   assert(y >= 0);
///   assert(y < CellColumn::Height);
///   return cells[y];  
/// }




bool Map::legalPos(CPoint pos) {
  if (pos.x < 0 || pos.y < 0) { return false; }
  if (pos.x >= Width2 || pos.y >= Height2) { return false;  }
  return true;
}



void Map::addRandomMob(int levelbase) {
  const int maxRetries = 75;
  for (int i = 0; i < maxRetries; ++i) {
    CPoint pos(rnd::Rnd(1, Width2), rnd::Rnd(1, Height2));
    assert(legalPos(pos));
    if (!(*this)[pos].creature.empty()) { continue; }
    addRandomMobAtPos(pos, levelbase);
    return;
  }
}


bool Map::addRandomMobAtPos(CPoint pos, int levelbase) {
  Cell& cell = (*this)[pos];
  if (!cell.creature.empty()) { debstr() << "cell already has mob.\n"; return false; }

  int mlevel = Levelize::suggestLevel(levelbase);
  Mob* monster = new MonsterMob(mlevel); // , this); // in addRandomMobAtPos.
  monster->placeMobOnMap(this);
  CreatureEnum ctype = MobDist::suggRndMob(mlevel); // Then pick an appropriate creature-type for that mob.
  monster->m_mobType = ctype;

  CL->map.moveMob(*monster, monster->pos);
  CL->mobs.queueMob(monster, 1);
  return true;
}


void Map::scatterMobsAtPos(CPoint pos, int n, int level, int radius) {
  for (int i = 0; i < n; ++i) {
    CPoint posA = pos;

    const int maxRetries = 10;
    for (int j = 0; j < maxRetries; ++j) {
      posA.x += rnd::rndC(-radius, radius);
      posA.y += rnd::rndC(-radius, radius);
      if (!legalPos(posA)) { continue; }
      if ((*this)[posA].blocked()) { continue; }
      if (!(*this)[posA].creature.empty()) { continue; }
      addRandomMobAtPos(posA, level);
      break;
    }
  }
}


void Map::addRandomObj(int level) {
  CPoint pos(rnd::Rnd(1, Width2), rnd::Rnd(1, Height2));
  assert(legalPos(pos));
  addObjAtPos(pos, level);
}


void Map::addObjAtPos(CPoint pos, int levelbase) {
  assert(legalPos(pos));
  Cell& cell = (*this)[pos];
  if (!cell.item.empty()) { debstr() << "cell already has item.\n"; return; }

  const ObjDef& ranDef = Obj::randObjDesc();
  int ilevel = Levelize::suggestLevel(levelbase);
  Obj* newObj = new Obj(ranDef, ilevel);

  if (newObj->otype() == OB_Lamp) {
    newObj->itemUnits += rnd::Rnd(500, 2500);
  }

  if (newObj->otype() == OB_WinItem && levelbase < 39) { // You can't find it before level 39..
    newObj->objdef = &Obj::objDesc(OB_Food);
  }
  cell.item.setObj(newObj); // in addObjAtPos.
}


void Map::scatterObjsAtPos(CPoint pos, int n, int level, int radius) {
  for (int i = 0; i < n; ++i) {
    CPoint posA = pos;
    posA.x += rnd::rndC(-radius, radius);
    posA.y += rnd::rndC(-radius, radius);
    addObjAtPos(posA, level);
  }
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
  }
  else {
    // alternate between the two:
    switch (level % 4) {
    case 0: initTunnels(level); break;
    case 1: initTunnelsBrown(level); break;
    case 2: initKachingBlob(level); break;
    case 3: initTunnels2(level); break;
    }
  }
}

/* done: place monsters on empty squares.
done: place myself on empty.
*/

void Map::initOuterBorders() {
  // husk 1-offset.
  for (int x = 0; x < Width2; ++x) {
    CPoint posTop(x, 0); // was: 1
    CPoint posBottom(x, Height2 - 1);
    Cell& cell1 = (*this)[posTop];    cell1.envir.type = EN_Border;
    Cell& cell2 = (*this)[posBottom]; cell2.envir.type = EN_Border;
  }

  for (int y = 0; y < Height2; ++y) { // was : 1 (old hack)
    CPoint posLeft(0, y);
    CPoint posRight(Width2 - 1, y);
    Cell& cell1 = (*this)[posLeft];  cell1.envir.type = EN_Border;
    Cell& cell2 = (*this)[posRight]; cell2.envir.type = EN_Border;
  }
}







void Map::initTown(int level) {

  const char* layout[] = {
    // "&", // First row for messages (fixme with viewport offset.)
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
    "&............&&...>...............G.....TT......&",
    "&............&&................T................5",
    "&...............................................&",
    "&.................TT............................6",
    "&...............................................&",
    "&................T..............................7",
    "&...............................................&",
    "&...................T.....T.....................&",
    "&...............................................&",
    "&...............................................&",
    "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&",
    ""
  };
  for (int y = 0; y < Height2 && layout[y][0] != '\0'; ++y) {
    const char* row = layout[y];
    for (int x = 0; x < Width2 && row[x] != '\0'; ++x) {
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

  SpellEnum spell = (SpellEnum)(x % SP_MaxSpells);
  Obj* newObj = new Obj(def, 1);
  newObj->effect = spell;
  cell.item.setObj(newObj); // in addColDemo (colour magic identify stuff.)
}


void Map::addStairs(int theWidth, int theHeight) {
  int numStairs = 3;
  for (int i = 0; i < numStairs; ++i) {
    addStair(EN_StairDown, theWidth, theHeight);
    addStair(EN_StairUp, theWidth, theHeight);
  }
}

void Map::addStair(EnvirEnum type, int theWidth, int theHeight) {
  CPoint stairPos = findFreeEnvir(EN_Floor, theWidth, theHeight);
  if (stairPos.x < 0) { return; } // give up.
  Cell& stairCell = (*this)[stairPos];
  stairCell.envir.type = type;
}

CPoint Map::findFreeEnvir(EnvirEnum type, int theWidth, int theHeight) {
  const int limit = 100;
  for (int i = 0; i < limit; ++i) {

    CPoint cand(
      rnd::Rnd(1, theWidth - 1), //Map::Width - 1),
      rnd::Rnd(1, theHeight - 1) //Map::Height-1)// FIXME, remember width+height independent!
      );

    if ((*this)[cand].envir.type == type) { return cand; }
  }
  CPoint notFound(-1, 1);
  return notFound;
}



CPoint Map::findNextEnvir(CPoint start, EnvirEnum type) {
  for (CPoint pos = start;;) { // (endless loop)
    ++pos.x;
    if (pos.x >= Width2) {
      pos.x = 0;
      ++pos.y;
      if (pos.y >= Height2) {
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
    if (pos.x >= Width2) {
      pos.x = 0;
      ++pos.y;
      if (pos.y >= Height2) {
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


void Map::clearMob(class Mob& m) {
  (*this)[m.pos].creature.clearMob();
}

void Map::moveMobNoInv(class Mob& m, CPoint newpos) { // , bool bInvalidate) {
  moveMobImpl(m, newpos, false);
}

void Map::moveMob(class Mob& m, CPoint newpos) { // , bool bInvalidate) {
  moveMobImpl(m, newpos, true);
}

void Map::setMobForce(class Mob& m, CPoint newpos, bool bInvalidate) {
  // used for swapping.
  CPoint oldpos = m.pos;
  m.pos = newpos;
  (*this)[m.pos].creature.setMob(&m);
  if (bInvalidate) {
    m.invalidateGfx(newpos, oldpos, false);
  }
  if (m.isPlayer()) {
    adjustViewport(newpos, bInvalidate);
  }
}

void Map::moveMobImpl(class Mob& m, CPoint newpos, bool bInvalidate) {
  CPoint oldpos = m.pos;
  (*this)[m.pos].creature.clearMob();
  m.pos = newpos;
  (*this)[m.pos].creature.setMob(&m);

  if (bInvalidate) {
    m.invalidateGfx(newpos, oldpos, false);
  }

  if (m.isPlayer()) {
    adjustViewport(newpos, bInvalidate);
  }
}


void Map::adjustViewport(CPoint newpos, bool bInvalidate) {
  Viewport::vp.adjust(newpos, *this);

  bool bChangedPos = !!(lightmap.map_offset != newpos);
  if (bChangedPos) {
    LightMap oldMap = lightmap;
    lightmap.map_offset = newpos;
    LOS::los.recalcLOS(lightmap);
    // Todo: is some sort of invalidategfx.. necessary?
    if (bInvalidate) {
      oldMap.invalidateDiff(lightmap);
    }
  }
}




void Map::addObj(class Obj& o, CPoint pos) {
  (*this)[pos].item.setObj(&o);
}


Viewport Viewport::vp;

Viewport::Viewport() {
  const int sweetspotWidth  = (SweetspotPct * VP_Width / 100);
  const int sweetspotHeight = (SweetspotPct * VP_Height / 100);
  sweetspotArea.top = sweetspotHeight;
  sweetspotArea.left = sweetspotWidth;
  sweetspotArea.right = VP_Width - sweetspotWidth;
  sweetspotArea.bottom = VP_Height - sweetspotHeight;
}



bool Viewport::adjust(CPoint wpos, Map& map) { // True if adjust happens.
  VPoint vp; 
  vp.p = w2v(wpos); //wpos - offset;
  if (sweetspotArea.PtInRect(vp.p)) { return false; } // No adjustment necessary.

  // Move offset, so wpos is in center of screen.
  CPoint oldOffset = offset;

  if (vp.p.x < sweetspotArea.left || vp.p.x > sweetspotArea.right) {
    int halfVPWidth = (VP_Width / 2);
    offset.x = wpos.x - halfVPWidth;

    if (wpos.x < halfVPWidth) { offset.x = 0; } // If we are close to left edge, lock it.
    if (wpos.x > (map.Width2 - halfVPWidth)) { offset.x = (map.Width2 - VP_Width); } // If we are close to right edge, lock it.
  }

  if (vp.p.y < sweetspotArea.top || vp.p.y > sweetspotArea.bottom) {
    int halfVPHeight = (VP_Height / 2);
    offset.y = wpos.y - halfVPHeight;

    if (wpos.y < halfVPHeight) { offset.y = 0; } // If we are close to left edge, lock it.
    if (wpos.y > (map.Height2 - halfVPHeight)) { offset.y = (map.Height2 - VP_Height); } // If we are close to bottom edge, lock it.
  }

  /* If we are 'cornered', we don't really want center; 
  instead we want to fill viewport.
  How can I describe this more completely.. 
  ( a correct description will probably illustrate how to impl.)

  It's something about being close to corners..
  If the distance to a corner is smaller than.. "half-screen-height?", 
  we should probably just stay there?
  */

  if (offset != oldOffset) {
    Cuss::invalidate();
  }
  return true; // We adjusted..
}




bool Map::persist(Persist& p) {

  // NB, currently all maps are the same size!
  p.transfer(Width2, "mapWidth");
  p.transfer(Height2, "mapHeight");
  if (!p.bOut) { resizeMap(); }

  int objCount = 0; // (count obj's, to aid later output of obj-list.)
  // First, output floor-cells:

  for (int y = 0; y < Height2; ++y) {
    for (int x = 0; x < Width2; ++x) {
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

  // FIXME - beware of any left traces of 'y-start = 1'

  p.transfer(objCount, "objCount");

  if (p.bOut) {
    // Output objects:
    for (int x = 0; x < Width2; ++x) {
      // CellColumn& column = (*this)[x];
      for (int y = 0; y < Height2; ++y) {
        CPoint pos(x, y);
        Cell& cell = (*this)[pos]; // cells[column[y];
        if (!cell.item.empty()) {
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


