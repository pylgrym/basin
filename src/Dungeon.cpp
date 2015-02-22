#include "stdafx.h"
#include "Dungeon.h"

#include "Mob.h"

Dungeon::Dungeon(int level_)
  :level(level_) 
{
}


Dungeon::~Dungeon()
{
}



void Dungeon::initDungeon() {
  map.initWorld(level);

  // Populate world:
  initPlayerForMap();
  initMobs();
}


void Dungeon::initPlayerForMap() { // JG, FIXME: All this shouldn't really clutter Map/CellMap -'initWorld'  should go somewhere OUTSIDE basic structures' impls.
  // FIXME: new + 'firstmove'+queue should become a single function.
  /* Figure out dependency order of map, creatures, queues, etc.*/

  // Mob* player = PlayerMob::createPlayer();

  //fixme
  /* fixme: now with multiple levels, player must hook up to
  each dungeon correctly,
  including picking an open cell.
  we only need to init once for each level.
  */
  PlayerMob* player = PlayerMob::ply;

  map.moveMob(*player, player->pos);
  mobs.queueMob(player, 0);
}


void Dungeon::initMobs() {
  const int mobCount = 10; // 100;
  for (int i = 0; i<mobCount; ++i) {
    debstr() << "i:" << i << "\n";

    Mob* monster = new MonsterMob( rndC( level, this->level+2 ) );
    map.moveMob(*monster, monster->pos);
    mobs.queueMob(monster, 1);
  }

  /*
  const int itemCount = 10; // 100;
  for (int i = 0; i < itemCount; ++i) {
  debstr() << "i:" << i << "\n";

  CPoint pos;
  pos.x = rnd(1, Map::Width-1);
  pos.y = rnd(1, Map::Height-1);
  if (CL->map[pos].blocked()) { continue; } // Don't add item on blocked floor.

  Obj* obj = new Obj(OB_Gold); // (L"Lamp");

  // color = RGB(rand()%255,rand()%255,rand()%255);
  CL->map.addObj(*obj, pos);
  }
  */

}


