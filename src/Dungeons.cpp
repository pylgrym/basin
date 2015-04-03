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
    newDungeon->initDungeon(); // Will move player in place! (on the map.)
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


Dungeon* Dungeons::setCurLevel(int level) {
  if (CL != NULL && PlayerMob::ply != NULL) { CL->map.clearMob(*PlayerMob::ply); } // Clear player from prev. level.

  Dungeon* dung = get(level);
  Dung::CL = dung;
  return dung;
}

/*I would like the log  to have more lines on the screen.. I think?
i would like..?
dragon-breath spells.
near-teleporting slowing fly?
something that teleports near, puts a touch-dot on you,
and teleports away..
  possibly something that steals your items, 
and either puts them somewhere else in the dungeon,
or you have to kill it/threaten it, to get your item back..
some radar-thing, that will only tell you distance or direction,
but not be precise?
  i wanted to change something..

i should have better stats, less clumsy.
my current char on other laptop, tells me how much armour is too much
(+12-13 gives me 80% resistance./AC protection.)

i still have that bug, where it displays a top-msg only partially?

nr + the kobold notices you?
I wanted to draw the keyboard!
 ½ 1 2 3 4 5 6 7 8 9 0 +(?) 
Tab Q W E R T Y U I O P Å ¨
     A S D F G H I J K L Æ Ø '
    < Z X C V B N M , . -
*/

void Dungeons::initNewGame() {

  // JG: NB! We don't need to account for player pos - dungeon will handle this, in initDungeon.
  PlayerMob* player = PlayerMob::createPlayer(); // nullMap); // Map* nullMap = NULL;
  Dungeon* dung = Dungeons::setCurLevel(player->dungLevel);

  /* I have been thinking a lot about whether createPlayer or 'createDungeonLevel (MAP)
  should come first. 
     My current thinking is, that 'player is more important than any given map,
  so even though I "could" hack it so a dungoen is created immediately before,
  to cater for the player, I .. won't.
    The real issue is, that assignment of position is tied wrongly to player creation
   - instead it should be delayed,
  to the  QUEUING of the mob (which is when it is tied to a given Dungeon = [map+mobs].)
  */
}

bool Dungeons::initLoadGame() {
  // Consider using LoadCmd here..:
  const char* file = "basin.sav";
  std::ifstream is(file);
  if (!is.good() || is.bad()) { return false; }

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
