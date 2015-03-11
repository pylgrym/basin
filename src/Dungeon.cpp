#include "stdafx.h"
#include "Dungeon.h"
#include "Mob.h"
#include "Bag.h"
#include "PlayerMob.h"
#include "Levelize.h"
#include "MobDist.h"



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


void Dungeon::initPlayerForMap() { 
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

  map.moveMob(*player, player->pos);
  mobs.queueMob(player, 0);
}


void Dungeon::initMobs() {
  int area = (Map::Width-2) * (Map::Height-2);
  int numMobs = area / 70; // 25; // one mob every 25 squares..

  const int mobCount = numMobs; // 10; // 100;
  for (int i = 0; i<mobCount; ++i) {
    debstr() << "i:" << i << "\n";

    int mlevel = Levelize::suggestLevel(this->level); // First, pick a level for new mob.
    CreatureEnum ctype = MobDist::suggRndMob(mlevel); // Then pick an appropriate creature-type for that mob.

    Mob* monster = new MonsterMob(mlevel); 
    monster->m_mobType = ctype;
    
    map.moveMob(*monster, monster->pos);
    mobs.queueMob(monster, 1);
  }

}


bool Dungeon::persist(class Persist& p) {
  mobs.persist(p);
  map.persist(p);

  if (!p.bOut) {
    ReadyQueue::iterator i;
    for (i = mobs.queue.begin(); i != mobs.queue.end(); ++i) {
      MobReady& mr = *i;
      map.moveMob(*mr.mob, mr.mob->pos);
    }
  }

  return true;
}
