#include "stdafx.h"
#include "Obj.h"

#include <string>
#include <vector>

#include "util/debstr.h"

#include "Levelize.h"

bool IsVowel(TCHAR c) {
  CString vowels = L"aeiouy";
  int pos = vowels.Find(c);
  return (pos >= 0);
}

std::string Obj::an_item() const { // based on some_item.
  CString s = some_item();
  TCHAR first = s[2];
  bool isVowel = IsVowel(first);
  if (isVowel) {
    s.Replace(L".", L"an"); // a/an for vowels..
  } else {
    s.Replace(L".", L"a"); // a/an for vowels..
  }

  s.Replace(L"-", L"some"); // FIXME - a/an for vowels..
  return std::string(CT2A(s));
}

std::string Obj::the_item() const { // based on some_item.
  CString s = some_item();
  s.Replace(L".", L"the");  
  return std::string(CT2A(s));
}

std::string Obj::indef_item() const { // based on some_item.
  CString s = some_item();
  s.Replace(L". ", L"");  
  return std::string(CT2A(s));
}


CString Obj::some_item() const {
  //const char* sA = typeAsDescA(otype());
  const char* sA = objdef->desc; 

  CA2T udesc(sA);
  CString s = udesc;
  CA2T uspell(Spell::type2desc(effect), CP_ACP); // CP_UTF8);

  const char* aslot = Equ::slotDesc(eqslot);
  if (eqslot == EQ_None) { aslot = "";  } // Don't display 'unwearable', it's superfluous.
  CA2T uslot(aslot, CP_ACP); // CP_UTF8);

  CString fmt;
  fmt.Format(L"%s of %s %dd%d(%d,%d) %s[%d]", (const TCHAR*)s, (const TCHAR*)uspell, dmgDice.n, dmgDice.x, toHit, toDmg, (const TCHAR*) uslot, ac);

  return fmt;
}




ObjDef objDefs[] = {
{OB_None,      EQ_None,    "nothing"}, 
{OB_Lamp,      EQ_None,    ". lamp"}, 
{OB_Sword,     EQ_MainHand,". sword"}, 
{OB_Hat,       EQ_Head,    ". hat"}, 
{OB_Gold,      EQ_None,    "- gold"},
{OB_Potion,    EQ_None,    ". potion"},
{OB_Scroll,    EQ_None,    ". scroll"},
{OB_Staff,     EQ_None,    ". staff"},
{OB_Wand,      EQ_None,    ". wand"},
{OB_Amulet,    EQ_Neck,    ". amulet"},
{OB_Food,      EQ_None,    "- food"},
{OB_Mushroom,  EQ_None,    ". mushroom"},
{OB_Shield,    EQ_OffHand, ". shield"},
{OB_Ring,      EQ_FingerLeft,". ring"}, // FIXME, rings can be worn both left and right.
{OB_Cloak,     EQ_Back,    ". cloak"},
{OB_Water,     EQ_None,    "- water"},
{OB_Bandage,   EQ_None,    ". bandage"},
{OB_Helmet,    EQ_Head,    ". helmet"},
{OB_ChestArmor,EQ_Chest,   ". breastplate"},
{OB_SpellBook, EQ_None,    ". spell book"},
{OB_Key,       EQ_None,    ". key"},
{OB_Candle,    EQ_None,    ". candle"},
{OB_Trap,      EQ_None,    ". trap"},
{OB_Gems,      EQ_None,    ". gem" },
{OB_LampOil,   EQ_None,    "- lamp oil" }, // Make &/! for a / some.
{OB_Rune,      EQ_None,    ". rune" },
{OB_Gloves,    EQ_Hands,   ". pair of gloves" },      // Gauntlets/gloves
{OB_Wrists,    EQ_Wrists,  ". pair of bracers"},     // bracers
{OB_Belt,      EQ_Waist,   ". belt"},                 //,=11,      // belt
{OB_Leggings,  EQ_Legs,    ". pair of leggings"},      // = 13,       // Pants/leggings.
{OB_Boots,     EQ_Feet,    ". pair of boots" },           //  = 14,       // Boots/shoes/sandals
{OB_Pickaxe,   EQ_2Hands,  ". pickaxe"},
{OB_Emeralds,  EQ_None,    ". emerald"},
{OB_Amethysts, EQ_None,    ". amethyst"},

// fixme -weapons need proficiencies or similar (maybe just level they appear on?
// to allow user to utilize the stronger weapons.
// weapons from  dnd4.wikia.com/wiki/Weapon
{ OB_Weapon, EQ_MainHand, ". club",        1, "1d6",1.5},
{ OB_Weapon, EQ_MainHand, ". dagger",      1, "1d4",0.5},
{ OB_Weapon, EQ_MainHand, ". javelin",     5, "1d6",1  },
{ OB_Weapon, EQ_MainHand, ". light mace",  3, "1d6",1  },
{ OB_Weapon, EQ_MainHand, ". mace",        5, "1d8",3  },
{ OB_Weapon, EQ_MainHand, ". short spear", 2, "1d6",0.5},
{ OB_Weapon, EQ_MainHand, ". sickle",      2, "1d6",1  },
{ OB_Weapon, EQ_MainHand, ". spear",       5, "1d8",6  },

{ OB_Weapon, EQ_MainHand, ". greatclub",    1, "2d4",5 },
{ OB_Weapon, EQ_MainHand, ". morningstar", 10,"1d10",4 },
{ OB_Weapon, EQ_MainHand, ". quarterstaff", 5, "1d8",2 },
{ OB_Weapon, EQ_MainHand, ". scythe",       5, "2d4",5 },

{ OB_Weapon, EQ_MainHand, ". battleaxe",    15,"1d10",3  },
{ OB_Weapon, EQ_MainHand, ". broadsword",   20,"1d10",2.5},
{ OB_Weapon, EQ_MainHand, ". flail",        10,"1d10",2.5},
{ OB_Weapon, EQ_MainHand, ". handaxe",       5, "1d6",1.5},
{ OB_Weapon, EQ_MainHand, ". khopesh",       5, "1d8",4  },
{ OB_Weapon, EQ_MainHand, ". lance",         5, "1d10",5 },
{ OB_Weapon, EQ_MainHand, ". light war pick ",5,"1d6",2  },
{ OB_Weapon, EQ_MainHand, ". longsword",     5, "1d8",2  },
{ OB_Weapon, EQ_MainHand, ". rapier",        5, "1d8",1  },
{ OB_Weapon, EQ_MainHand, ". scimitar",      5, "1d8",2  },
{ OB_Weapon, EQ_MainHand, ". scourge",       5, "1d8",1  },
{ OB_Weapon, EQ_MainHand, ". short sword",   5, "1d6",1  },
{ OB_Weapon, EQ_MainHand, ". throwing hammer",5,"1d6",1  },
{ OB_Weapon, EQ_MainHand, ". trident",       5, "1d8",2  },
{ OB_Weapon, EQ_MainHand, ". warhammer",     5, "1d10",2.5},
{ OB_Weapon, EQ_MainHand, ". war pick",      5, "1d8",3  },

{ OB_Weapon, EQ_MainHand, ". falchion",     25, "2d4" ,3.5},
{ OB_Weapon, EQ_MainHand, ". glaive",       25, "2d4" ,5 },
{ OB_Weapon, EQ_MainHand, ". greataxe",     30, "1d12",6 },
{ OB_Weapon, EQ_MainHand, ". greatsword",   30, "1d10",4 },
{ OB_Weapon, EQ_MainHand, ". halberd",      25, "1d10",6 },
{ OB_Weapon, EQ_MainHand, ". heavy flail",  25, "2d6" ,5 },
{ OB_Weapon, EQ_MainHand, ". heavy war pick",20,"1d12",4 },
{ OB_Weapon, EQ_MainHand, ". long spear",   10, "1d10",4.5},
{ OB_Weapon, EQ_MainHand, ". maul",         30, "2d6" ,6 },
{ OB_Weapon, EQ_MainHand, ". pike",         15, "1d10",3 },

{ OB_Weapon, EQ_MainHand, ". bastard sword",   30, "1d10", 3 },
{ OB_Weapon, EQ_MainHand, ". bola",             5, "1d4",  1 },
{ OB_Weapon, EQ_MainHand, ". craghammer",      20, "1d10", 3 },
{ OB_Weapon, EQ_MainHand, ". katar",            3, "1d6",0.5 },
{ OB_Weapon, EQ_MainHand, ". kukri",           10, "1d6",  1 },
{ OB_Weapon, EQ_MainHand, ". net",             20, "1d6",  3 },
{ OB_Weapon, EQ_MainHand, ". parrying dagger",  5, "1d4",0.5 },
{ OB_Weapon, EQ_MainHand, ". serrated pick",   15, "1d10", 2 },
{ OB_Weapon, EQ_MainHand, ". tratnyr",         10, "1d8",2.5 },
{ OB_Weapon, EQ_MainHand, ". triple-headed flail", 15, "1d10",3},
{ OB_Weapon, EQ_MainHand, ". waraxe",          30, "1d12", 5 },
{ OB_Weapon, EQ_MainHand, ". whip",            10, "1d4",  1 },

{ OB_Weapon, EQ_MainHand, ". execution axe", 30, "1d12",7 },
{ OB_Weapon, EQ_MainHand, ". fullblade", 30, "1d12",5 },
//{ OB_Weapon, EQ_MainHand, ". garrote", 1, "1d4",0.5},
{ OB_Weapon, EQ_MainHand, ". gouge", 30, "2d6",6 },
{ OB_Weapon, EQ_MainHand, ". greatspear", 25, "1d6",4 },
{ OB_Weapon, EQ_MainHand, ". mordenkrad", 30, "2d6",6 },
{ OB_Weapon, EQ_MainHand, ". spiked chain", 30, "2d4" ,5},

{ OB_Weapon, EQ_MainHand, ". double axe", 40, "1d6",7.5 },
{ OB_Weapon, EQ_MainHand, ". double flail", 30, "1d6", 5.5},
{ OB_Weapon, EQ_MainHand, ". double sword", 40, "1d6",4.5 },
{ OB_Weapon, EQ_MainHand, ". urgrosh", 30, "1d12",4 },

{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
};


const TCHAR* Obj::otypeAsStr(ObjEnum type) {
  /* JG: This was/is used for the tilemap-keyname assoc.:
  */
  static std::vector<CString> thingKeys;
  if (thingKeys.size() == 0) {
    thingKeys.resize(OB_MaxLimit);
    // thingKeys[CR_None] = L"notInMap"; 
    thingKeys[OB_Lamp] = L"~"; 
    thingKeys[OB_Sword] = L"|"; 
    thingKeys[OB_Hat] = L"& Hard Leather Cap";  // 26,11 // & Hard Leather Cap
    thingKeys[OB_Gold] = L"$";  

    thingKeys[OB_Potion] = L"!";
    thingKeys[OB_Scroll] = L"?";
    thingKeys[OB_Staff] = L"staff"; // "|";
    thingKeys[OB_Wand] = L"-";
    thingKeys[OB_Amulet] = L"amulet"; // \"";
    thingKeys[OB_Food] = L"food"; // '";
    thingKeys[OB_Mushroom] = L"mushroom"; // '";
    thingKeys[OB_Shield] = L")";
    thingKeys[OB_Ring] = L"=";
    thingKeys[OB_Cloak] = L"cloak"; // ^";
    thingKeys[OB_Water] = L"water"; // o";
    thingKeys[OB_Bandage] = L"bandage"; // %";
    thingKeys[OB_Helmet] = L"helmet"; // &";
    thingKeys[OB_ChestArmor] = L"(";

    thingKeys[OB_SpellBook] = L"spellbook";
    thingKeys[OB_Key] = L"key";
    thingKeys[OB_Candle] = L"candle";
    thingKeys[OB_Trap] = L"trap";
    thingKeys[OB_Gems] = L"gem";
    thingKeys[OB_LampOil] = L"oil";
    thingKeys[OB_Rune] = L"rune";

    // I wish to switch from this to ObjDef system.
    thingKeys[OB_Gloves] = L"gloves";
    thingKeys[OB_Wrists] = L"bracers";
    thingKeys[OB_Belt] = L"belt";
    thingKeys[OB_Leggings] = L"leggings";
    thingKeys[OB_Boots] = L"boots";
    thingKeys[OB_Pickaxe] = L"pickaxe";
    thingKeys[OB_Emeralds] = L"emeralds";
    thingKeys[OB_Amethysts] = L"amethysts";
  }

  if (type < 0 || type >= (int) thingKeys.size()) { return L"out of bounds, thing enum.";  }
  return thingKeys[type];
}


const ObjDef& Obj::objDesc(ObjEnum type) {
  static const ObjDef dummy = { OB_None, EQ_None, "bad obj" };
  const int numObjDefs = (sizeof objDefs / sizeof ObjDef);
  if (type >= 0 && type < numObjDefs) {
    ObjDef* od = &objDefs[type]; // WARNING - contents of objDefs no longer satisfy this! (ix is not type)
    return *od;
  }
  debstr() << "warning/error, bad objEnum?!\n";
  return dummy;
}

const ObjDef& Obj::randObjDesc() {
  const int numObjDefs = (sizeof objDefs / sizeof ObjDef);
  int ix = rnd(numObjDefs);
  ObjDef* od = &objDefs[ix];
  return *od;
}


const char* Obj::flavorUse(ObjEnum type) {
  switch (type) {
  case OB_Potion:   return "You quaff the potion."; // drink, sip, ?
  case OB_Water:    return "You drink the water."; // drink, sip, ?
  case OB_Scroll:   return "You read the scroll."; // drink, sip, ?

  case OB_Food:     return "You eat the food."; // drink, sip, ?
  case OB_Mushroom: return "You eat the mushroom."; // drink, sip, ?
  }
  return "You use the thingey.";
}


const char* Obj::typeAsDescA(ObjEnum type) { // not currently used!
  const ObjDef& od = objDesc(type);
  //const int numObjDefs = (sizeof objDefs / sizeof ObjDef);
  //if (type >= 0 && type < numObjDefs) {
  //  ObjDef* od = &objDefs[type];
  //}
  return od.desc;
  /*
  switch (type) {     
  case OB_None:      return L"nothing"; 
  case OB_Lamp:      return L". lamp"; 
  case OB_Sword:     return L". sword"; 
  case OB_Hat:       return L". hat"; 
  case OB_Gold:      return L". gold";
  case OB_Potion:    return L". potion";
  case OB_Scroll:    return L". scroll";
  case OB_Staff:     return L". staff";
  case OB_Wand:      return L". wand";
  case OB_Amulet:    return L". amulet";
  case OB_Food:      return L". food";
  case OB_Mushroom:  return L". mushroom";
  case OB_Shield:    return L". shield";
  case OB_Ring:      return L". ring";
  case OB_Cloak:     return L". cloak";
  case OB_Water:     return L". water";
  case OB_Bandage:   return L". bandage";
  case OB_Helmet:    return L". helmet";
  case OB_ChestArmor:return L". chest armor";

  case OB_SpellBook: return L". spell book";
  case OB_Key:       return L". key";
  case OB_Candle:    return L". candle";
  case OB_Trap:      return L". trap";
  case OB_StairUp:   return L". stair up";
  case OB_StairDown: return L". stair down";
  case OB_Rune:      return L". rune";

  }
  */
  return "obj error";
}


ObjEnum ObjSlot::type() const {
  return o != NULL ? o->otype() : OB_None;
}



bool Obj::wearable() const {
  return (eqslot != EQ_Unwearable); // Equippable);
}


int Obj::getLightStrength() const {
  if (otype() != OB_Lamp) { return 1;  } // No light from a non-lamp.
  return itemUnits / 25; // 100; // 1500 units will give you 15.
}

void Obj::burnUnits(int units) {
  int remainingUnits = itemUnits - units;
  if (remainingUnits < 0) { remainingUnits = 0;  }
  itemUnits = remainingUnits;
}

int Obj::digStrength() const {
  if (otype() == OB_Pickaxe) {
    return 200;
  }
  if (otype() == OB_Sword) {
    return dmgDice.roll(std::stringstream());
  }

  return 4; // Sort of like bare hands.
}
 


void Obj::initRandom() { // - clear should not init.
  charges = rnd(-1, 7); // 3);
  consumed = oneIn(2); // true;
  toHit = rndC(-2, 5);
  toDmg = rndC(-2, 6);
  weight = rnd(1, 50);
  itemUnits = rnd(20, 400);

  // Dice(rndC(1, 4), rndC(2, 12));
  dmgDice = Levelize::randDiceForLevel(ilevel);
  ac = Levelize::suggestLevel(ilevel);
}
