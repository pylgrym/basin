#include "stdafx.h"
#include "Dungeon.h"
#include "Mob.h"
// #include "Bag.h"
#include "PlayerMob.h"
#include "Levelize.h"
#include "MobDist.h"



Dungeon::Dungeon(int level_)
:level(level_) 
,map(Map::DefWidth, Map::DefHeight)
{
}


Dungeon::~Dungeon()
{
}



void Dungeon::initDungeon() {
  map.initWorld(level);

  // Populate world:
  initPlayerForMap(); // in initDungeon.
  initMobs();
}


void Dungeon::initPlayerForMap() { // called by initDungeon.

  // JG, FIXME: All this shouldn't really clutter Map/CellMap
  // -'initWorld'  should go somewhere OUTSIDE basic structures' impls.

  // FIXME: new + 'firstmove'+queue should become a single function.
  /* Figure out dependency order of map, creatures, queues, etc.*/

  // Mob* player = PlayerMob::createPlayer();
  
  //
  /* beware: now with multiple levels, player must hook up to
  each dungeon correctly,
  including picking an open cell.
  we only need to init once for each level.
  */
  PlayerMob* player = PlayerMob::ply;

  CPoint freePos = map.findNextEnvir(player->pos, EN_Floor);
  player->pos = freePos;

  map.moveMob(*player, player->pos);
  mobs.queueMob(player, 0);
}


void Dungeon::initMobs() {
  int area = (map.Width2-2) * (map.Height2-2);
  int numMobs = area / 70; // 25; // one mob every 25 squares..

  const int mobCount = numMobs; // 10; // 100;
  for (int i = 0; i<mobCount; ++i) {
    debstr() << "i:" << i << "\n";

    int mlevel = Levelize::suggestLevel(this->level); // First, pick a level for a fresh mob.
    CreatureEnum ctype = MobDist::suggRndMob(mlevel); // Then pick an appropriate creature-type for that mob.

    Mob* monster = new MonsterMob(mlevel); // , &map);
    monster->placeMobOnMap(&map);
    monster->m_mobType = ctype;
    
    map.moveMob(*monster, monster->pos);
    mobs.queueMob(monster, 1);
  }

}


bool Dungeon::persist(class Persist& p) {
  map.persist(p);
  mobs.persist(p); // , &map); // JG: I've changed the order here, because I want the map ready before the mobs.

  /* JG, consider: Do we want this to happen 'inside' the mobqueue, and not 'out' here in the dungeon code?
  */
  if (!p.bOut) {
    ReadyQueue::iterator i;
    for (i = mobs.queue.begin(); i != mobs.queue.end(); ++i) {
      MobReady& mr = *i;
      map.moveMobNoInv(*mr.mob, mr.mob->pos); // "No-Invalidate."
    }
  }

  return true;
}
