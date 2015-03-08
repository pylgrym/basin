#pragma once

#include <ostream>

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
  SC_Phys=0,
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
  SP_Speedup ,
  SP_Slowdown ,
  SP_Confuse ,
  SP_Unconfuse, 
  SP_ConfuseMob,
  SP_Teleport  ,
  SP_MagicMissile , // SC_Magic
  SP_FireBolt ,     // SC_Fire
  SP_FrostBolt ,    // SC_Frost
  SP_FireBall ,     // SC_Fire
  SP_StoneToMud ,   // SC_Air/Elm? // "DONE"
  SP_WallBuilding , // SC_Earth    // "DONE"
  SP_Earthquake ,   // SC_Earth    // Todo - different kind of spell! (it's like a 'mass digging'.)
  SP_StinkCloud ,   // SC_Poison/SC_Gas
  SP_Eat ,
  SP_Heal_light,
  SP_Heal_minor,
  SP_Heal_mod,
  SP_Heal_serious,
  SP_Heal_crit,
  SP_Sick,
  SP_LightArea , // "DONE"
  SP_LightDir ,  // "DONE"
  SP_MagicMap,
  SP_PhaseDoor,
  SP_DetectDoor,
  SP_DetectTrap,
  SP_DetectTreasure,
  SP_DetectObject,
  // SP_x
  SP_MaxSpells
};

struct SpellDesc {
  int level;
  int manaCost;
  const char* abbr;
  const char* desc;
  // const char* bullet;
  std::string qual;
  COLORREF color;
  bool ident; // known purpose or not.
  bool ability; // a learned ability.
};



class Spell {
public:
  Spell();
  ~Spell();
  static const char* type2str(SpellEnum);

  static const char* type2desc_Id(SpellEnum);       // ('IDed')    Will always return 'identified'/proper name.
  static const char* type2desc_Mys(SpellEnum type); // ('Mystery') Will return a 'quality' if not identified.

  static const char* bulletTxt(SpellEnum effect);

  static SpellEnum str2type(const char* str);
  static const SpellDesc& spell(SpellEnum);
  static SpellDesc& spellNC(SpellEnum);
  static SpellEnum rndSpell();

  static bool doSpell(SpellEnum, class Mob& actor, std::ostream& log, class Obj* item); // Mob& target, 

  static void initQual();

  static void trySpellIdent(SpellEnum effect);
  static int manaCost(SpellEnum effect);

  static bool persist(class Persist& p);

private:
  static SpellDesc spells[SP_MaxSpells];

};

