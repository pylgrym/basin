#pragma once

#include <ostream>
#include <set>
#include "numutil/myrnd.h"
#include <map>

/* JG: Spell/Spellnum allows me to map enums to text-tags and back.
This makes it possible for me to have sets of spell effects on items.

  Right now, relation between Commands and Spells is not totally resolved;
they are cousins.. Commands are, sort of, spells,
and some commands will trigger spells.
  A meeting/overlap, is that the 'Use' command, on an item,
will trigger the spell effect of the item.
  Note, that spells, like commands, may have arbitrary 'owner' and 'victim/target'
(both player and mob.)
*/


enum AttackSchool {
  // Ideas for different kinds of attack, for weakness/strength against.
  SC_None=0,
  SC_Phys=1,
  SC_Fire,
  SC_Frost,
  SC_Water,
  SC_Lightning,
  SC_Air,
  SC_Earth,
  SC_Acid,
  SC_Shadow,
  SC_Magic,
  SC_Nature,
  SC_Undead,
  SC_Holy,
  SC_Poison,
  SC_Gas,
  SC_Mind,
  SC_Light,
  SC_MaxSchools
};

/*
  CPoint tileMagic(18, 24);  SC_Magic
  CPoint tileFire(22, 24);   SC_FIre
  CPoint tileFrost(23, 24);  SC_Frost
  CPoint tilePurple(34, 24);
  CPoint tileYellow(35, 24); SC_Earth
  CPoint tileGreenFire(39, 24); 
  CPoint tileGreenBall(3, 25); SC_Gas/SC_Poison
*/

enum SpellEnum {
  SP_NoSpell = 0,

  SP_Speedup,
  SP_Slowdown,
  SP_ConfuseSelf,
  SP_Unconfuse, 
  SP_ConfuseMob,

  SP_SleepOther,
  SP_TeleSelfAway,
  SP_TeleOtherAway, // teleother, tele_other, teleportother, teleport_other.

  SP_SummonHere, // mob x goes next to me.
  SP_SummonMonster, // create random monster.
  SP_SummonObj, // create random item.
  SP_TeleportTo, // I go next to mob x.
  SP_TeleSwap, // swap places..
  SP_MagicMissile, // SC_Magic // FIXME - no AOE.
  SP_StinkCloud ,   // SC_Poison/SC_Gas
  SP_FireBolt ,     // SC_Fire
  SP_FrostBolt ,    // SC_Frost
  SP_FireBall ,     // SC_Fire  10
  SP_StoneToMud ,   // SC_Air/Elm? // "DONE"
  SP_WallBuilding , // SC_Earth    // "DONE"
  SP_Earthquake ,   // SC_Earth    // Todo - different kind of spell! (it's like a 'mass digging'.)
  SP_FocusBlast, // hit hard, exactly one space away.
  SP_Rush, // charge - you launch YOURSELF as a projectile, and bump into the mob. so - both projectile and 'teleport' (works similarly to teleport-to)
  SP_Crush, // crush mob between hard wall and yourself (requires a wall on the other side.)
  SP_Embed, // Push mob INTO the rock..
  SP_Shove, // push mob sliding along the floor, to bumb into far wall.
  SP_Tackle, // push both yourself and mob sliding along the floor, bumping into wall.
  SP_Breath, // meant for dragons, not for you..
  // SP_Claw,
  SP_Eat,
  SP_Heal_light,
  SP_Heal_minor,
  SP_Heal_mod,
  SP_Heal_serious,
  SP_Heal_crit, // 20
  SP_Sick,
  SP_LightArea , // "DONE"
  SP_LightDir ,  // "DONE"
  SP_MagicMap,   // 24
  SP_PhaseDoor,  // 25
  SP_DetectDoor,
  SP_DetectTrap,
  SP_DetectTreasure,
  SP_DetectObject, // 30
  SP_DetectMobs,
  SP_RestoreMana,
  // SP_x
  SP_MaxSpells
};

struct DiceStruct {
  int num;
  int side;
};

struct SpellDesc {
  int level;
  int manaCost;
  DiceStruct dice;
  int minRange, maxRange; // NB! -only used by 'zap' projectile spells, for now.
  AttackSchool school;
  const char* abbr;
  const char* desc;

  // const char* bullet;
  std::string qual;
  COLORREF color;
  bool ident; // known purpose or not.
  bool ability; // a learned ability.
};




class SpellParam { 
public:
  SpellParam() {
   impl = NULL; // ptr to class below..
   actor = NULL;
   target = NULL;
   dir = CPoint(0,0);  // zap bullet dir.
   factor = 1.0; // speed factor.
   tmpEffect = 0; // Nr of confuse-turns, might be 0.
   range = 0; //  teleport range.
   deltaFood = 0; 
   healPct = 0;
   healDice =Dice(1,1);
   manaPct = 0;

   item = NULL;
   effect = SP_NoSpell;
   school = SC_None;

   pos = CPoint(0,0); // light area
   radius = 0; // light area
  }

  class SpellImpl* impl; // ptr to class below..
  class Mob* actor;
  Mob* target;
  CPoint dir;  // zap bullet dir.
  double factor; // speed factor.

  int tmpEffect; // confuse; // Nr of confuse-turns, might be 0.

  int range; //  teleport range.
  int deltaFood; 
  int healPct;
  Dice healDice;
  int manaPct;

  class Obj* item;
  SpellEnum effect;
  AttackSchool school;

  CPoint pos; // light area
  int radius; // light area

  bool exec();
};


class SpellImpl {
public:
  static std::set<SpellImpl*> spellColl;
  static std::map< std::string, SpellImpl* > spellMap;

  SpellImpl() { spellColl.insert(this); }
  ~SpellImpl() { spellColl.erase(this); } // erase is the opposite of insert.

  virtual std::string spelltag() const = 0;

  virtual bool getParams(SpellParam& param) { return true; }
  virtual bool execSpell(SpellParam& param) = 0;

  static void initSpellMap();
  static SpellImpl* spellFromTag(const std::string& tag);
};


enum ManaEnum { NoMana = 0, UseMana = 1 };

class Spell {
public:
  enum Nums { SpellPageSize = 15 }; // 19}; // 20 }; // 5 is good for testing..

  Spell();
  ~Spell();
  static const char* type2str(SpellEnum);

  static const char* type2desc_Id(SpellEnum);       // ('IDed')    Will always return 'identified'/proper name.
  static const char* type2desc_Mys(SpellEnum type); // ('Mystery') Will return a 'quality' if not identified.

  static const char* bulletTxt(SpellEnum effect);

  static SpellEnum str2type(const char* str);
  static const SpellDesc& spell(SpellEnum);
  static SpellDesc& spellNC(SpellEnum);

  static bool legalSpellIx(int ix);

  static CPoint bulletTileForSchool(AttackSchool school); // Gives you the bullet-gfx-tile to use for a given type of magic.

  static SpellEnum rndSpell_dangerous();
  static SpellEnum rndSpell_level(int ilevel);

  static bool prepareSpell(SpellParam& p, SpellEnum, class Mob& actor, Mob* target, class Obj* item); 
  static bool castSpell(SpellEnum, class Mob& actor, Mob* target, class Obj* item, const ManaEnum useMana); 

  static SpellEnum pickASpell(const char* prompt);
  static SpellEnum pickSpellAction(int& offset, bool& cancel); // accept user input
  static void showSpellInv(int offset, int numItems);

  static CPoint pickZapDir();
  static CPoint NoDir; // 0,0

  static void initQual();

  static void trySpellIdent(SpellEnum effect);
  static int manaCost(SpellEnum effect);

  static bool isDmgSpell(SpellEnum stype);

  static bool manaCostCheck(SpellEnum stype, Mob& mob, std::ostream& err);

  static bool persist(class Persist& p);

private:
  static SpellDesc spells[SP_MaxSpells];

};

