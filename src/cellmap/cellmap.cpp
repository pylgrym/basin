#include "stdafx.h"
#include "./cellmap.h"

#include "numutil/myrnd.h"

#include "../Mob.h"

#include <assert.h>

#include "Bag.h"

void Map::addRandomMob() {
  CPoint pos(rnd(1, Width), rnd(2, Height));
  assert(legalPos(pos));
  Cell& cell = (*this)[pos];
  if (!cell.creature.empty()) { debstr() << "cell already has mob.\n"; return; }

  Mob* monster = new MonsterMob;
  Map::map.moveMob(*monster, monster->pos);
  MobQueue::mobs.queueMob(monster,1);
}



void Map::addRandomObj() {
  CPoint pos(rnd(1, Width), rnd(2, Height));
  assert(legalPos(pos));
  addObjAtPos(pos);
}

void Map::addObjAtPos(CPoint pos) {
  assert(legalPos(pos));
  Cell& cell = (*this)[pos];
  if (!cell.item.empty()) { debstr() << "cell already has item.\n"; return; }
  ObjEnum otype = (ObjEnum) rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);
  cell.item.setObj(new Obj(otype));
}

void Map::scatterObjsAtPos(CPoint pos, int n) {
  for (int i = 0; i < n; ++i) {
    CPoint posA = pos;
    posA.x += rndC(-1, 1);
    posA.y += rndC(-1, 1);
    addObjAtPos(posA);
  }
}



Map::Map() {}

void Map::initWorld() {

  // Create floor/environ.
  for (int x = 0; x < Width; ++x) {
    CellColumn& column = (*this)[x];
    for (int y = 1; y < Height; ++y) {
      Cell& cell = column[y];
      if (x == 0 || y == 1 || x == Width - 1 || y == Height - 1) { // The outer border.
        cell.envir.type = EN_Border;
      } else { // Inside-area:
        bool isWall = oneIn(3);  
        cell.envir.type = (isWall ? EN_Wall : EN_Floor);

        if (!isWall) {
          bool hasThing = oneIn(9);  
          if (hasThing) {
            // bool type2 = oneIn(3);  
            ObjEnum otype = (ObjEnum) rnd(1, OB_MaxLimit); // (type2 ? OB_Lamp : OB_Sword);
            cell.item.setObj(new Obj(otype));
          }
        }
        /*
        bool hasCreature = oneIn(17);  
        if (hasCreature) {
          bool type2 = oneIn(3);  
          cell.creature.type = (type2 ? CR_Kobold : CR_Dragon);
        }
        */
      }
    }
  } // for x/ create floor.

  // Populate world:
   
  initPlayer();

  const int mobCount = 10; // 100;
  for (int i=0; i<mobCount; ++i) {
    debstr() << "i:" << i << "\n";

    Mob* monster = new MonsterMob;
    Map::map.moveMob(*monster, monster->pos);
    MobQueue::mobs.queueMob(monster,1);
  }

  /*
  const int itemCount = 10; // 100;
  for (int i = 0; i < itemCount; ++i) {
    debstr() << "i:" << i << "\n";

    CPoint pos;
    pos.x = rnd(1, Map::Width-1); 
    pos.y = rnd(1, Map::Height-1);
    if (Map::map[pos].blocked()) { continue; } // Don't add item on blocked floor.

    Obj* obj = new Obj(OB_Gold); // (L"Lamp");

    // color = RGB(rand()%255,rand()%255,rand()%255);
    Map::map.addObj(*obj, pos);
  }
  */


}


void Map::initPlayer() { // JG, FIXME: All this shouldn't really clutter Map/CellMap -'initWorld'  should go somewhere OUTSIDE basic structures' impls.
    // FIXME: new + 'firstmove'+queue should become a single function.
  /* Figure out dependency order of map, creatures, queues, etc.*/

  Mob* player = new PlayerMob; // Is a singleton, will store himself.
  Map::map.moveMob(*player, player->pos);
  MobQueue::mobs.queueMob(player,0);

  std::stringstream ignore;
  Bag::bag.add(new Obj(OB_Hat),ignore);
  Bag::bag.add(new Obj(OB_Sword),ignore);
  // Bag::bag.add(new Obj(OB_Gold),ignore);
  Bag::bag.add(new Obj(OB_Lamp),ignore);
  Bag::bag.add(new Obj(OB_Hat),ignore);

  Bag::bag.add(new Obj(OB_Potion),ignore);
  Bag::bag.add(new Obj(OB_Scroll),ignore);
  Bag::bag.add(new Obj(OB_Food),ignore);
  Bag::bag.add(new Obj(OB_Pickaxe),ignore);
}



Map Map::map;


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

  debstr() << "adjust..\n";
  // Move offset, so wpos is in center of screen.
  offset.x = wpos.x - (Width / 2);
  offset.y = wpos.y - (Height / 2);
  return true; // We adjusted..
}


