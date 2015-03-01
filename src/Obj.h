#pragma once

#include <string>

#include "LogEvents.h"

#include "Spell.h"
#include "Equ.h"

#include "numutil/myrnd.h"

#include "Persist.h"


enum ObjEnum {
  OB_None=0,
  OB_Lamp=1,
  OB_Sword=2,
  OB_Hat=3,
  OB_Gold=4,
  OB_Potion=5,
  OB_Scroll=6,
  OB_Staff=7,
  OB_Wand=8,
  OB_Amulet=9,
  OB_Food=10,
  OB_Mushroom=11,
  OB_Shield=12,
  OB_Ring=13,
  OB_Cloak=14,
  OB_Water=15,
  OB_Bandage=16,
  OB_Helmet=17,
  OB_ChestArmor=18,
  OB_SpellBook=19,
  OB_Key=20,
  OB_Candle=21,
  OB_Trap=22,
  OB_Gems = 23,
  OB_LampOil = 24, // FIXME/TODO: make item to refuel lamp!

  OB_Rune=25,
  OB_Gloves=26,
  OB_Wrists=27,
  OB_Belt=28,
  OB_Leggings=29,
  OB_Boots=30,
  OB_Pickaxe=31,
  OB_Emeralds=32,
  OB_Amethysts=33,
  OB_Weapon = 34,
  /// OB_StairUp = 34,
  /// OB_StairDown = 35,

  // Idea: Make a 'wonder' lamp that will 'lightning-flicker' once in a while, temporarily revealing bigger map.
  // Idea: light/lamps that colour the dungeon with coloured light, for flavor.
  OB_MaxLimit // highest nr to size arrays.
};

struct ObjDef { // For declaring POD struct sets. Intention is to eventually read from files instead of compiled source.
  ObjEnum type;
  EquipSlotEnum eqtype;
  const char* desc;
  int price;
  const char* defDice;
  double kilo;
};


class Obj {
public:
  const ObjDef* objdef;
  //ObjEnum oldtype; // Don't use this, use objdef.
  int ilevel;

  bool persist(Persist& p, CPoint& pos);

  virtual int price() const { return objdef->price;  }

  SpellEnum effect;
  // std::set < SpellEnum > ;  // JG: Might become a set instead of a single effect.
  EquipSlotEnum eqslot() const { return (objdef ? objdef->eqtype : EQ_None ); }
  int weight; // tenths of kilos, 100g. // fixme, should be virtual func from objdef.
  int itemUnits; // food'charges', light'charges', gold 'charges', etc.
  int ac; // armour item armour class.

  // BEGIN BEHAVIOUR
  int charges; // -1 means infinite. (use-charges) (food, potions, scrolls will be 1.)
  bool consumed; // Will be consumed/destroyed at zero charges.

  // ATTACK/Damage compotent:
  int toHit; // tohit modifier.
  int toDmg; // todamage modifier.
  Dice dmgDice; // The damage this object/weapon can deal.

  virtual ObjEnum otype() const { 
    return objdef ? objdef->type : OB_Lamp; // oldtype;
  }


  Obj(const ObjDef& objdef_, int level_) :objdef(&objdef_)
    ,ilevel(level_), effect(SP_NoSpell), weight(0), itemUnits(0), ac(1)  
  {
    effect = Spell::rndSpell();
    const ObjDef& desc = objdef_; 

    clear(); 
    initRandom();
  }

  std::string an_item() const; 
  std::string the_item() const; 
  std::string indef_item() const; 
  CString some_item() const; 

  bool wearable() const;

  int getLightStrength() const;
  void burnUnits(int units);

  int digStrength() const;

  void clear() { // - clear should not init (to random) - use initRandom instead.
    charges = 1; 
    consumed = true;
    toHit = 0; // rndC(-2, 5);
    toDmg = 0; // rndC(-2, 6);
    dmgDice = Dice(1, 2); // rndC(1, 4), rndC(2, 12));
    weight = 1; // rnd(1, 50);
    itemUnits = 0;
  }

  void initRandom();

  void setTypeDefaults();


  virtual bool use(class Mob& who, std::ostream& err) { // returns true if use succeeded.
    /* FIXME/TODO: multi-messages must 'prompt with <more>',
    ie whenever too much info to print, guide the user through it.

    Also, make monsters attack.
    */
    const char* flavor = Obj::flavorUse(otype()); // ObjEnum type) {
    { // JG, FIXME - why doesn't this display?
      logstr log; log << flavor;
    }

    if (!infiniteCharges()) {
      if (!eatCharge(err)) { return false; }
    }

    logstr log;
    // Act on obj.effect:
    Spell::doSpell(effect, who, log, this);

    return true;
  } 

  bool eatCharge(std::ostream& err) {
    if (charges == 0) { err << "It doesn't have any charges left.";  return false; }
    --charges; 

    logstr log;
    log << charges << " charges remain. ";
    return true;
  }
  bool infiniteCharges() const { return (charges == -1);  }
  // END BEHAVIOUR

  static const ObjDef& randObjDesc();
  static const ObjDef& randObjDesc2();

  static const ObjDef& objDesc(ObjEnum type);

  static int def2ix(const ObjDef* objdef);

  static const char* objdefAsStr(const ObjDef& def);
  static const TCHAR* not_used_otypeAsStr(ObjEnum type);
  // static const TCHAR* typeAsDescU(ObjEnum type);
  static const char* typeAsDescA(ObjEnum type);

  static const char* flavorUse(ObjEnum type);

  static std::string make_indef_item(const std::string& s);
};



// JG: You could argue whether ObjSlot should be here on in cellmap or a third place.
class ObjSlot { // We have two layers, so we can have virtuals - otherwise, Obj could just be a 'value' type.
public:
  virtual ObjEnum type() const;
  virtual const ObjDef& objDef() const;
  Obj* o;
  ObjSlot():o(NULL) {} 

  bool persist(class Persist& p) {
    p.os << (void*) o;
    return true;
  }

  void setObj(Obj* o_) { o = o_; }

  // const TCHAR* oldtypeS() { return Obj::otypeAsStr(type()); }
  // const TCHAR*    utypeS() { return Obj::objdefAsStr(objDef()); }
  const char*    atypeS()  { return Obj::objdefAsStr(objDef()); }

  bool empty() const { return type() == OB_None; }

};
