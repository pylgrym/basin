#pragma once

#include <string>

#include "LogEvents.h"

#include "Spell.h"
#include "Equ.h"

#include "numutil/myrnd.h"


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
  OB_StairUp=23,
  OB_StairDown=24,
  OB_Rune=25,

  OB_Gloves=26,
  OB_Wrists=27,
  OB_Belt=28,
  OB_Leggings=29,
  OB_Boots=30,
  OB_Pickaxe=31,
  OB_Emeralds=32,
  OB_Amethysts=33,
  OB_Gems=34,
  OB_MaxLimit // highest nr to size arrays.
};

struct ObjDef { // For declaring POD struct sets. Intention is to eventually read from files instead of compiled source.
  ObjEnum type;
  EquipSlotEnum eqtype;
  const char* desc;
};


class Obj {
public:
  ObjEnum type;

  SpellEnum effect;
  // std::set < SpellEnum > ;  // JG: Might become a set instead of a single effect.
  EquipSlotEnum eqslot;
  int weight; // tenths of kilos, 100g.
  int itemUnits; // food'charges', light'charges', gold 'charges', etc.

  virtual ObjEnum otype() const { return type; }

  Obj() :type(OB_None), effect(SP_NoSpell), eqslot(EQ_Unwearable), weight(0),itemUnits(0) { clear(); }

  Obj(ObjEnum type_) :type(type_), effect(SP_NoSpell), eqslot(EQ_Unwearable), weight(0), itemUnits(0) {
    effect = Spell::rndSpell();

    const ObjDef& desc = Obj::objDesc(type);
    if (desc.eqtype != EQ_None) {
      eqslot = desc.eqtype; // Equ::rndSlot();
    } else {
      eqslot = desc.eqtype; // JG, FIXME: I could shorten this to just assign it always..
    }

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

  void initRandom() { // FIXME - clear should not init.
    charges = rnd(-1,3); 
    consumed = oneIn(2); // true;
    toHit = rndC(-2, 5);
    toDmg = rndC(-2, 6);
    dmgDice = Dice(rndC(1, 4), rndC(2, 12));
    weight = rnd(1, 50);
    itemUnits = rnd(20, 400);
  }

  // BEGIN BEHAVIOUR
  int charges; // -1 means infinite. (use-charges) (food, potions, scrolls will be 1.)
  bool consumed; // Will be consumed/destroyed at zero charges.

  // ATTACK/Damage compotent:
  int toHit; // tohit modifier.
  int toDmg; // todamage modifier.
  Dice dmgDice; // The damage this object/weapon can deal.

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
    Spell::doSpell(effect, who, log);

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

  static const ObjDef& objDesc(ObjEnum type);

  static const TCHAR* typeAsStr(ObjEnum type);
  static const TCHAR* typeAsDescU(ObjEnum type);
  static const char* typeAsDescA(ObjEnum type);

  static const char* flavorUse(ObjEnum type);
};



// JG: You could argue whether ObjSlot should be here on in cellmap or a third place.
class ObjSlot { // We have two layers, so we can have virtuals - otherwise, Obj could just be a 'value' type.
public:
  virtual ObjEnum type() const;
  Obj* o;
  ObjSlot():o(NULL) {} 

  void setObj(Obj* o_) { o = o_; }

  const TCHAR* typeS() { return Obj::typeAsStr(type());  }

  bool empty() const { return type() == OB_None; }

};
