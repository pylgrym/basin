#pragma once

#include <ostream>

#include "Persist.h"
#include <vector>

enum CreatureEnum {
  CR_None=0,
  CR_Player=1,
  CR_Kobold=2,
  CR_Dragon=3,
  CR_FireAnt=4,
  CR_Jelly=5,
  CR_Eye=6,
  CR_Beholder=7,
  CR_Gargoyle=8,
  CR_Lich=9,
  CR_Wraith=10,
  CR_Imp=11,
  CR_IronGolem=12,
  CR_YellowSlime=13,
  CR_BlueSlime=14,
  CR_EarthGolem,
  CR_FireSprite,
  CR_Goblin,
  CR_GoblinWar,
  CR_NetherSnail,
  CR_ShadowSnail,
  CR_Rat,
  CR_Squirrel,
  CR_BlueCentipede,
  CR_RedScorpion,
  CR_GreenWorms,
  CR_PurpleCentipede,
  CR_GreenBeetle,
  CR_HypnoMoth,
  CR_Sparkly,
  CR_RottingDevourer,
  CR_SilverWyrm,
  CR_BrownBat,
  CR_BlackBat,
  CR_VeiledHorror,
  CR_GreyDragon,
  CR_AzureDragon,
  CR_RedDragon,
  CR_OrangeDragon,
  CR_BlueDragon,
  CR_GreenDragon,
  CR_YellowDragon,
  CR_FireElemental,
  CR_WaterElemental,
  CR_MaxLimit // highest nr to size arrays.
};


struct MobDef {
  int mlevel;
  CreatureEnum type;
  const char* tilekey;
  const char* desc;

  std::vector< const char* > tags;

  // const char* tags;
  // (const char* tags)[];
  //(const char* tags)[];
  //const char* tag[32]; // [];
};


class Creature {
public:
  CreatureEnum type() const;
  class Mob* m;
  Creature():m(NULL){}

  bool persist(class Persist& p) {
    p.os << (void*)m;
    return true;
  }

  const char* typeS() { return ctypeAsStr(type());  }

  bool empty() const { return type() == CR_None; }
  bool blocked() const { return isBlockingCreature(type());  }

  void clearMob() { m = NULL; }  
  void setMob(Mob* m_) { m = m_; }

  static const MobDef&  mobDef(CreatureEnum type);
  static const char* ctypeAsStr(CreatureEnum type);
  static const char* ctypeAsDesc(CreatureEnum type);

  static bool isBlockingCreature(CreatureEnum type);
};
