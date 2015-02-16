#include "stdafx.h"
#include "Creature.h"

#include <vector>

#include "Mob.h"

bool Creature::isBlockingCreature(CreatureEnum type) {
  switch (type) {
  case CR_None:
    return false; // not blocking.
    // everything else is blocking movement..
  }

  return true;
}


CreatureEnum Creature::type() const {
  return m != NULL ? m->ctype() : CR_None;
}






MobDef mobDefs[] = {
  { CR_None, ".", "nothing" },
  { CR_Player, "@", "you" },
  { CR_Kobold, "k", ". kobold" },
  { CR_Dragon, "d", ". dragon" },
  { CR_FireAnt, "a", ". fire ant" },
  { CR_Jelly, "J", ". jelly" },
  { CR_Eye, "e", ". floating eye" },
  { CR_Beholder, "beholder", ". beholder" },
  { CR_Gargoyle, "gargoyle", ". gargoyle" },
  { CR_Lich, "lich", ". lich" },
  { CR_Wraith, "wraith", ". wraith" },
  { CR_Imp, "imp", ". imp" },
  { CR_IronGolem, "irongolem", ". iron golem" },
  { CR_YellowSlime, "yellowslime", ". yellow slime" },
  { CR_BlueSlime, "blueslime", ". blue slime" },
  { CR_EarthGolem, "earthgolem", ". earth golem" },
  { CR_FireSprite, "firesprite", ". fire sprite" },
  { CR_Goblin, "goblin", ". goblin" },
  { CR_GoblinWar, "goblinwar", ". goblin warrior" },
  { CR_NetherSnail, "nethersnail", ". nether snail" },
  { CR_ShadowSnail, "shadowsnail", ". shadow snail" },
  { CR_Rat, "rat", ". rat" },
  { CR_Squirrel, "squirrel", ". squirrel" },
  { CR_BlueCentipede, "bluecentipede", ". blue centipede" },
  { CR_RedScorpion, "redscorpion", ". red scorpion" },
  { CR_GreenWorms, "greenworms", ". green worms" },
  { CR_PurpleCentipede, "purplecentipede", ". purple centipede" },
  { CR_GreenBeetle, "greenbeetle", ". green beetle" },
  { CR_HypnoMoth, "hypnomoth", ". hypnomoth" },
  { CR_Sparkly, "sparkly", ". sparkly" },
  { CR_RottingDevourer, "rottingdevourer", ". rotting devourer" },
  { CR_SilverWyrm, "silverwyrm", ". silverwyrm" },
  { CR_BrownBat, "brownbat", ". brown bat" },
  { CR_BlackBat, "blackbat", ". black bat" },
  { CR_VeiledHorror, "veiledhorror", ". veiled horror" },
  { CR_GreyDragon, "greydragon", ". grey dragon" },
  { CR_AzureDragon, "azuredragon", ". azure dragon" },
  { CR_RedDragon, "reddragon", ". red dragon" },
  { CR_OrangeDragon, "orangedragon", ". orange dragon" },
  { CR_BlueDragon, "bluedragon", ". blue dragon" },
  { CR_GreenDragon, "greendragon", ". green dragon" },
  { CR_YellowDragon, "yellowdragon", ". yellow dragon" },
  { CR_FireElemental, "fireelemental", ". fire elemental" },
  { CR_WaterElemental, "waterelemental", ". water elemental" },
};


const MobDef&  Creature::mobDef(CreatureEnum type) {
  static MobDef badMob = { CR_None, "§", "bad mtype" };
  const int maxMob = (sizeof mobDefs / sizeof MobDef);
  if (type < 0 || type >= maxMob) { return badMob; } //  L"out of bounds, creature enum.";
  return mobDefs[type]; 
}

const char* Creature::typeAsStr(CreatureEnum type) { // TCHAR
  /*
  static std::vector<CString> creatureKeys;
  if (creatureKeys.size() == 0) {
    creatureKeys.resize(CR_MaxLimit);
    // creatureKeys[CR_None] = L"notInMap"; 
    creatureKeys[CR_Player] = L"@"; 
    creatureKeys[CR_Kobold] = L"k"; 
    creatureKeys[CR_Dragon] = L"d"; 
  }
  */
  const int maxMob = (sizeof mobDefs / sizeof MobDef);
  if (type < 0 || type >= maxMob) { return "out of bounds, creature enum.";  } // (int) creatureKeys.size()
  return mobDefs[type].tilekey; // creatureKeys[type];
}


const char* Creature::ctypeAsDesc(CreatureEnum type) {
  /*
  switch (type) {
  case CR_None:   return L". nothing"; 
  case CR_Player: return L"you"; 
  case CR_Kobold: return L". kobold"; 
  case CR_Dragon: return L". dragon"; 
  }
  return L". creature";
  */
  const int maxMob = (sizeof mobDefs / sizeof MobDef);
  if (type < 0 || type >= maxMob) { return "out of bounds, creature enum.";  } // (int) creatureKeys.size()
  return mobDefs[type].desc; // creatureKeys[type];
}
