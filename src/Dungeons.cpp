#include "stdafx.h"
#include "Dungeons.h"

#include "Mob.h"
#include "Bag.h"
#include "ShopInv.h"

#include "PlayerMob.h"
#include <assert.h>

Dungeons Dungeons::the_dungeons;

Dungeon* Dungeons::get(int level) {
  return the_dungeons.implGet(level);
}

Dungeon* Dungeons::implGet(int level) {
    // we start at level 1, not 0! (maybe there'll be a town level at 0.)
  if ((int)dungeons.size() <= level) {
    dungeons.resize(level+1);
  }
  if (dungeons[level] == NULL) {
    Dungeon* newDungeon = new Dungeon(level);
    newDungeon->initDungeon();
    dungeons[level] = newDungeon;
  }

  return dungeons[level];
}

bool Dungeons::persist(class Persist& p) {
  int dungLevel = 1;
  if (p.bOut) { dungLevel = PlayerMob::ply->dungLevel; }
  p.transfer(dungLevel, "dungLevel");

  Viewport::vp.persist(p);

  Bag::bag.persist(p);
  Equ::worn.persist(p);

  Spell::persist(p); // remember which spells we recognise, and which we have as abilities.

  ShopInv::shop.persist(p);

  int dungeonDepth = dungeons.size();
  p.transfer(dungeonDepth, "dungeonDepth");
  if (!p.bOut) { dungeons.resize(dungeonDepth);  }

  int dungeonCount = 0;
  for (int i = 0; i < (int) dungeons.size(); ++i) {
    if (dungeons[i] != NULL) {
      ++dungeonCount;
    }
  }
  p.transfer(dungeonCount, "dungeonCount");

  if (p.bOut) { // Output
    for (int i = 0; i < dungeonDepth; ++i) {
      if (dungeons[i] != NULL) {
        p.transfer(i, "dungeonLevel");
        Dungeon& dungeon = *dungeons[i];
        dungeon.persist(p);
      }
    }
  } else { // Input.
    for (int i = 0; i < dungeonCount; ++i) {
      int dungeonLevel = 0;
      p.transfer(dungeonLevel, "dungeonLevel");
      Dungeon* dungeon = new Dungeon(dungeonLevel);
      dungeons[dungeonLevel] = dungeon;
      dungeon->persist(p);
    }
  }


  if (!p.bOut) { 
    assert(PlayerMob::ply != NULL);
    PlayerMob::ply->dungLevel = dungLevel;
    setCurLevel(dungLevel);
  }

  return true;
}


Dungeon* Dung::CL = NULL; // should at least be in a namespace with 'using'.


void Dungeons::setCurLevel(int level) {
  CL->map.clearMob(*PlayerMob::ply); // Clear player from prev. level.
  Dungeon* dung = get(level);
  Dung::CL = dung;
}


void Dungeons::initNewGame() {
  PlayerMob* player = PlayerMob::createPlayer();
  Dungeons::setCurLevel(player->dungLevel);
}

bool Dungeons::initLoadGame() {
  // Consider using LoadCmd here:
  const char* file = "basin.sav";
  std::ifstream is(file);
  Persist p(is);
  bool bLoadOK = persist(p); // Dungeons::the_dungeons.

  // Kludge: not how we want to do it..
  // CL->map.lightmap.map_offset = PlayerMob::ply->pos;
  //LOS::los.recalcLOS(CL->map.lightmap);

  Spell::spellNC(SP_LightArea).ability = true; // Player starts out knowing light-area.. for now.

  extern void bresenExample();
  bresenExample(); // Just show how it looks.. :-)

  return bLoadOK;
}

void Dungeons::initDungeons(bool loadGame) {
  bool bDone = false;
  if (loadGame) {
    bDone = initLoadGame();
  }

  if (!bDone) {
    initNewGame();
  }

  // Hack - make sure we know MM:
  Spell::spellNC(SP_MagicMissile).ability = true; // Player starts out known magic-missile.

  PlayerMob::ply->passTime(); // Hack to make player-LIGHT init correctly; could be handled many other ways.

  // This is a little bit bad(?), because it even
  // atempts to trigger a redraw, at a time where we don't have any HWND yet..
}


Dungeons::Dungeons()
{
}


Dungeons::~Dungeons()
{
}
