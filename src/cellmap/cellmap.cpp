#include "stdafx.h"
#include "./cellmap.h"

#include "numutil/myrnd.h"

#include "../Mob.h"

#include <assert.h>

#include "Bag.h"

#include "../MobQueue.h"

#include "DungGen1.h"

#include "Levelize.h"

void Map::addRandomMob(int level) {
  CPoint pos(rnd(1, Width), rnd(2, Height));
  assert(legalPos(pos));
  Cell& cell = (*this)[pos];
  if (!cell.creature.empty()) { debstr() << "cell already has mob.\n"; return; }

  Mob* monster = new MonsterMob(level);
  CL->map.moveMob(*monster, monster->pos);
  CL->mobs.queueMob(monster,1);
}



void Map::addRandomObj(int level) {
  CPoint pos(rnd(1, Width), rnd(2, Height));
  assert(legalPos(pos));
  addObjAtPos(pos,level);
}

void Map::addObjAtPos(CPoint pos,int level) {
  assert(legalPos(pos));
  Cell& cell = (*this)[pos];
  if (!cell.item.empty()) { debstr() << "cell already has item.\n"; return; }

  const ObjDef& ranDef = Obj::randObjDesc();
  //ObjEnum otype = (ObjEnum) rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);
  Obj* newObj = new Obj(ranDef,level); // otype
  if (newObj->otype() == OB_Lamp) {
    newObj->itemUnits += rnd(500, 2500);
  }
  cell.item.setObj(newObj);
}

void Map::scatterObjsAtPos(CPoint pos, int n, int level) {
  for (int i = 0; i < n; ++i) {
    CPoint posA = pos;
    posA.x += rndC(-1, 1);
    posA.y += rndC(-1, 1);
    addObjAtPos(posA, level);
  }
}



Map::Map() {}


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

        cell.envir.type = etype; // (isWall ? EN_Wall : EN_Floor);

        if (etype == EN_Vein) {
          bool hasThing = oneIn(4);
          if (hasThing) {
            ObjEnum otype = OB_Gold; //  (ObjEnum)rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);
            const ObjDef& goldType = Obj::objDesc(OB_Gold);
            int ilevel = Levelize::suggestLevel(level);
            cell.item.setObj(new Obj(goldType,ilevel));
          }
        }

        if (!isWall) {
          bool hasThing = oneIn(9);  
          if (hasThing) {
            // TODO: ObjEnum must become ObjType/ObjCat, and ObjDef must become prominent. 
            //ObjEnum otype = (ObjEnum) rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);

            const ObjDef& objType = Obj::randObjDesc2(); 
            int ilevel = Levelize::suggestLevel(level);
            cell.item.setObj(new Obj(objType, ilevel));
          }
        }
      }
    }
  } // for x/ create floor.   

  addStairs();
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


//Map Map::map; // CL->map;


void Map::moveMob(class Mob& m, CPoint newpos) {
  (*this)[m.pos].creature.clearMob();
  m.pos = newpos;
  (*this)[m.pos].creature.setMob(&m);

  if (m.isPlayer()) {
    Viewport::vp.adjust(m.pos);
  }
}




void Map::addObj(class Obj& o, CPoint pos) {
  (*this)[pos].item.setObj(&o);
}



Cell& Map::operator [] (CPoint p) { 
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

  return true; // We adjusted..
}




bool Map::persist(Persist& p) {
  int objCount = 0; // (count obj's, to aid later output of obj-list.)
  // First, output floor-cells:
  for (int y = 1; y < Height; ++y) {
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
  cell.item.setObj(o);
  return true;
}
