#include "stdafx.h"
#include "Obj.h"

#include <string>
#include <vector>

#include "util/debstr.h"

#include "Levelize.h"

#include <assert.h>

#include "Cmds.h"
#include "PlayerMob.h"


bool IsVowel(TCHAR c) {
  CString vowels = L"aeiou"; // y is consonant, speaking-wise.
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

  s.Replace(L"- ", L"some "); // FIXME - a/an for vowels..
  return std::string(CT2A(s));
}

std::string Obj::the_item() const { // based on some_item.
  CString s = some_item();
  s.Replace(L".", L"the");  
  return std::string(CT2A(s));
}


std::string Obj::make_indef_item(const std::string& s) {
  CString sU = CA2T(s.c_str(), CP_ACP); // tmpU;
  sU.Replace(L". ", L"");
  sU.Replace(L"- ", L""); // grammatically, it makes sense not to do this.
  return std::string(CT2A(sU));
}

std::string Obj::indef_item() const { // based on some_item.
  CString s = some_item();
  s.Replace(L". ", L"");  
  s.Replace(L"- ", L"");  
  return std::string(CT2A(s));
}


CString Obj::some_item() const {
  // shorter - no article:
  std::string sFull = objdef->desc; // indef_item(); // would be recursive..
  std::string sRight = sFull.substr(2);
  std::string sArticle = sFull.substr(0, 2);

  CA2T udesc(sRight.c_str());
  CString desc = udesc;

  CA2T uarticle(sArticle.c_str());
  CString article = uarticle;

  const char* spellDesc = soloident ? Spell::type2desc_Id(effect) : Spell::type2desc_Mys(effect);
  CA2T uspell(spellDesc, CP_ACP); 

  const char* aslot = Equ::slotDesc(eqslot());
  if (eqslot() == EQ_None) { aslot = "";  } // Don't display 'unwearable', it's superfluous.
  CA2T uslot(aslot, CP_ACP); 

  CString fmt;
  // fmt.Format(L" %s %s %dd%d(%d,%d) [%d] <%d>", (const TCHAR*)desc, (const TCHAR*)uspell, dmgDice.n, dmgDice.x, toHit, toDmg, ac, charges);

  CString sAC;      if (eqslot() != EQ_None)     { sAC.Format(L"[%d]", ac); }
  CString sDice;    if (Equ::isWeapon(eqslot())) { sDice.Format(L"%dd%d", dmgDice.n, dmgDice.x); }
  CString sBonus;   if (Equ::isWeapon(eqslot())) { sBonus.Format(L"(%d,%d)", toHit, toDmg); }
  CString sCharges; if (usesCharges(otype()))    { sCharges.Format(L"<%d>", charges); }

  fmt.Format(L"%s%s %s %s%s%s%s", (const TCHAR*)article, (const TCHAR*)uspell, (const TCHAR*)desc, sAC, sDice, sBonus, sCharges);

  return fmt;
}




ObjDef objDefs[] = {
{OB_None,      EQ_None,    "nothing",           0, "1d1", 0.0 },
{OB_Lamp,      EQ_None,    ". lamp",            2, "1d2", 0.5 }, 
{OB_Sword,     EQ_MainHand,". sword",           7, "1d6", 1.5 }, // longsword: 1.5kg, claymore 2.5 
{OB_Hat,       EQ_Head,    ". hat",             3, "1d2", 0.5 }, 
{OB_Gold,      EQ_None,    "- gold",           25, "1d2", 1.5 },
{OB_Potion,    EQ_None,    ". potion",          3, "1d2", 0.2 }, // or even less, mainly to avoid tons of them? in practice, less than 0.1..
{OB_Scroll,    EQ_None,    ". scroll",          2, "1d2", 0.1 }, // scrolls should weight very little, however they might be BULKY, so count weight anyway.
{OB_Staff,     EQ_None,    ". staff",           4, "2d3", 2.0 },
{OB_Wand,      EQ_None,    ". wand",            2, "1d2", 0.2 }, // vej selv en istedet..
{OB_Amulet,    EQ_Neck,    ". amulet",          6, "1d2", 0.3 },
{OB_Food,      EQ_None,    "- food",            2, "1d2", 0.8 }, // 1.6lbs
{OB_Mushroom,  EQ_None,    ". mushroom",        2, "1d2", 0.4 },
{OB_Shield,    EQ_OffHand, ". shield",          4, "2d3", 7.5 },
{OB_Ring,      EQ_FingerLeft,". ring",          7, "1d2", 0.1 }, // FIXME, rings can be worn both left and right.
{OB_Cloak,     EQ_Back,    ". cloak",           4, "1d2", 2.5 },
{OB_Water,     EQ_None,    "- water",           1, "1d2", 1.0 },
{OB_Bandage,   EQ_None,    ". bandage",         1, "1d2", 0.5 },
{OB_Helmet,    EQ_Head,    ". helmet",          4, "1d3", 2.0 },
{OB_ChestArmor,EQ_Chest,   ". breastplate",     6, "1d3", 3.5 },
{OB_SpellBook, EQ_None,    ". spell book",      4, "1d2", 0.7 },
{OB_Key,       EQ_None,    ". key",             0, "1d2", 0.1 },
{OB_Candle,    EQ_None,    ". candle",          0, "1d2", 0.2 },
{OB_Trap,      EQ_None,    ". trap",            0, "1d2", 0.5 },
{OB_Gems,      EQ_None,    ". gem",             0, "1d2", 0.5 },
{OB_LampOil,   EQ_None,    "- lamp oil",        0, "1d2", 1.0 }, // Make &/! for a / some.
{OB_Rune,      EQ_None,    ". rune",            0, "1d2", 0.5 },
{OB_Gloves,    EQ_Hands,   ". pair of gloves",  0, "1d2", 0.5 },      // Gauntlets/gloves
{OB_Wrists,    EQ_Wrists,  ". pair of bracers", 0, "1d2", 1.0 },     // bracers
{OB_Belt,      EQ_Waist,   ". belt",            0, "1d2", 1.0 },                 //,=11,      // belt
{OB_Leggings,  EQ_Legs,    ". pair of leggings",0, "1d2", 1.5 },      // = 13,       // Pants/leggings.
{OB_Boots,     EQ_Feet,    ". pair of boots",   0, "1d2", 3.5 },           //  = 14,       // Boots/shoes/sandals
{OB_Pickaxe,   EQ_2Hands,  ". pickaxe",         8, "1d5", 3.5 },
{OB_Emeralds,  EQ_None,    ". emerald",        14, "1d2", 0.5 },
{OB_Amethysts, EQ_None,    ". amethyst",       12, "1d2", 0.5 },
{OB_WinItem,   EQ_None,    ". emulator of Rodney", 12, "8d9", 5.0 }, // You find it on level 39.

// fixme -weapons need proficiencies or similar (maybe just level they appear on?
// to allow user to utilize the stronger weapons.
// weapons from  dnd4.wikia.com/wiki/Weapon
{ OB_Weapon, EQ_MainHand, ". club",        1, "1d6",1.5 }, 
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

/* 
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
{ OB_Weapon, EQ_MainHand, ".", 5, "1d6" },
*/
};

const int numObjDefs = (sizeof objDefs / sizeof ObjDef);

void Obj::initPrices() {
  for (int i = 0; i < numObjDefs; ++i) {
    ObjDef& def = objDefs[i];
    if (def.price == 0) {
      def.price = rndC(1, 11);
    }
  }
}

void Obj::initWeights() {
  for (int i = 0; i < numObjDefs; ++i) {
    ObjDef& def = objDefs[i];
    if (def.kilo == 0) {
      def.kilo = rndC(1, 50) *0.1;
    }
  }
}

const ObjDef& Obj::randObjDescBad() { // will give too many weapons.
  int ix = rnd(numObjDefs);
  ObjDef* od = &objDefs[ix];
  return *od;
}

const ObjDef& Obj::randObjDesc() { // works better, on types instead.
  static std::map<ObjEnum, std::vector<ObjDef*> > descs;
  if (descs.size() == 0) { // Do first-time init.
    for (int i = 0; i < numObjDefs; ++i) {
      ObjDef& def = objDefs[i];
      descs[def.type].push_back(&def);
      if (def.price == 0) {
        def.price = rndC(1, 11);
      }
    }
  }

  ObjEnum ranType = (ObjEnum) rnd(OB_MaxLimit);
  std::vector<ObjDef*>& defs = descs[ranType];
  int choice = rnd(defs.size());
  ObjDef* od = defs[choice];
  return *od;
}


const char* Obj::objdefAsStr(const ObjDef& def) {
  const ObjDef* defptr = &def;
  int ix = defptr - &objDefs[0]; // if you subtract two ptrs, you get the array index.
  static std::vector< std::string > shortNames;
  if (shortNames.size() == 0) {
    const int numObjDefs = (sizeof objDefs / sizeof ObjDef);
    shortNames.resize(numObjDefs);
    // std::ofstream theset("theset.key");
    for (int i = 0; i < numObjDefs; ++i) {
      ObjDef& def = objDefs[i];
      std::string sName = Obj::make_indef_item(def.desc);
      shortNames[i] = sName;
      if (def.price == 0) {
        def.price = rndC(1, 11);
      }
      // theset << "\"" << sName << "\"" << " " << i % 40 << " 11\n";
    }
  }
  assert(ix >= 0);
  assert(ix < (int) shortNames.size());
  return shortNames[ix].c_str();
}

const TCHAR* Obj::not_used_otypeAsStr(ObjEnum type) {
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


int Obj::def2ix(const ObjDef* objdef) {
  if (objdef == NULL) { return -1; }

  const int numObjDefs = (sizeof objDefs / sizeof ObjDef);

  int ix = (objdef - &objDefs[0]);
  assert(ix >= 0);
  assert(ix < numObjDefs);    
  if (ix < 0 || ix >= numObjDefs) { ix = 0; }
  return ix;
}




const char* Obj::flavorUse(ObjEnum type) {
  switch (type) {
  case OB_Potion:   return "You quaff the potion."; // drink, sip, ?
  case OB_Water:    return "You drink the water."; // drink, sip, ?
  case OB_Scroll:   return "You read the scroll."; // drink, sip, ?

  case OB_Food:     return "You eat the food."; // drink, sip, ?
  case OB_Mushroom: return "You eat the mushroom."; // drink, sip, ?
  case OB_LampOil:  return NULL; // IE don't show flavor-text.
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

const ObjDef& ObjSlot::objDef() const {
  return o != NULL ? *o->objdef : Obj::objDesc(OB_None);
}




bool Obj::wearable() const {
  return (eqslot() != EQ_Unwearable); // Equippable);
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
  charges = rnd(-1, 7); 
  consumed = oneIn(2); 

  // Plusses need to become a lot rarer: (and more -levelizer?)
  toHit = Levelize::suggestExtra(ilevel); // rndC(-2, ilevel); // 5);
  toDmg = Levelize::suggestExtra(ilevel); // rndC(-2, ilevel); // 6);
  if (XinY(3,11)) { // Make more trash..
    toHit = -toHit;
  }
  if (XinY(3,11)) {
    toDmg = -toDmg;
  }

  itemUnits = rnd(20, 400);

  dmgDice = Levelize::randDiceForLevel(ilevel); // normal weapon-dmg comes from level machine, not from object defs.
  ac = Levelize::suggestLevel(ilevel);

  setTypeDefaults(); // Bring some sanity back..
}

void Obj::setTypeDefaults() {
  switch (otype()) {
    // Edible stuff and similar, have ONE charge and is consumed on use:
  case OB_Potion: case OB_Scroll: case OB_Water: case OB_Bandage: case OB_Food: case OB_Mushroom: case OB_LampOil:
    this->charges = 1;
    this->consumed = true;

    if (otype() == OB_Food) {
      if (oneIn(2)) { 
        effect = SP_Eat;  
        this->itemUnits = rnd(700, 4500);
      }
    }
    break;


  case OB_Staff: case OB_Wand:
    consumed = false; // 'chargey' items are not consumed on use-up, so far.
    charges = rnd(0,16);
    const SpellDesc& desc = Spell::spell(effect);
    dmgDice = Dice(desc.dice.num, desc.dice.side); // better: we get info from spell-def instead of extra code.
    // NB! this actualy has little effect, since it's the spell-desc that actually counts, 
    // and NOT the obj-properties (unless the obj is used as a melee weapon.)
    /*
    switch (effect) {
    case SP_MagicMissile: dmgDice = Dice(2, 4); break; // 5 // FIXME, this only makes OBJECTS hit hard, how about the general spell..
    case SP_StinkCloud:   dmgDice = Dice(3, 4); break; // 7.5 // FIXME, what levels..?
    case SP_FrostBolt:    dmgDice = Dice(4, 5); break; // 12  // I hope we are colouring monsters based on their spells, bcs. some hit hard..
    case SP_FireBolt:     dmgDice = Dice(5, 6); break; // 17.5
    case SP_FireBall:     dmgDice = Dice(6, 7); break; // 30
    case SP_StoneToMud:   dmgDice = Dice(7, 8); break; // 31.5 
    case SP_WallBuilding: dmgDice = Dice(8, 9); break; // 40
    case SP_Earthquake:   dmgDice = Dice(9,10); break; // 49.5
    case SP_FocusBlast:   dmgDice =  Dice(4,6); break; // ?49.5      
    }
    */
    break;
  }

  if (isCurrency(otype())) { // NB, 'gold' items shouldn't give too much gold.
    int rate = 5;
    switch (otype()) {
    case OB_Gems:      rate = 3;  break;
    case OB_Gold:      rate = 5;  break;
    case OB_Amethysts: rate = 9;  break; // FIXME/depending on 'growth slope', these should be adjusted.
    case OB_Emeralds:  rate = 12; break;
    }
    itemUnits = Dx( ilevel*rate + 1); // At level 10, expect to find up to 50g.
  }

  if (eqslot() != EQ_None) { // try-out hack:
    ac = rnd(1, 7) + (ilevel/2); // Item level will give us better items.
  }

}




bool learnSpell(SpellEnum effect, std::ostream& err, bool force) {
  SpellDesc& desc = Spell::spellNC(effect);

  if (desc.ability) { 
    err << "But you already know this spell!"; return false;
  }

  if (!desc.ident && !force) {
    err << "The purpose of this spell is a mystery."; return false;
  }

  if (desc.level > PlayerMob::ply->stats.level()) {
    err << "You cannot yet grasp this complex spell."; return false;
  }

  err << "You suddenly realize how this spell works!"; 
  desc.ability = true;
  if (force) {
    desc.ident = true; // "Force" means, that you both learn how to do the spell, and how to ID it.
  }
  return true;

}


bool Obj::useObj(class Mob& who, std::ostream& err) { // returns true if use succeeded.
  /* Multi-messages must 'prompt with <more>',
  ie whenever too much info to print, guide the user through it. */

  const char* flavor = Obj::flavorUse(otype()); 
  if (flavor != NULL) { 
    logstr log; log << flavor;
  }

  if (!infiniteCharges()) { // NB! - we can't eat the charge up front, because aiming-spells or risks might abort with 'cancel'.
    if (!anyChargesLeft()) { err << "It doesn't have any charges left."; return false; }
  }

  if (otype() == OB_LampOil) {
    bool bOK = FillLampCmd(this).Do(err);
    if (bOK) { eatCharge(err);  }
    return bOK;
  }

  if (otype() == OB_SpellBook && who.isPlayer()) { // Is this the right place? (eating-food is handled in doSpell.)
    if (learnSpell(effect, err, false)) {
      eatCharge(err);
      return true;
    }
    return false;
  }

  bool bOK = false;
  { // Act on obj.effect:
    logstr log;
    bOK = Spell::castSpell(effect, who, NULL, this, NoMana); // in Obj::useObj.
    if (bOK) {
      if (bOK && who.isPlayer()) {
        Spell::trySpellIdent(effect);
        // Now eat the charge (fixme - castSpell should indicate how we failed - user cancel or not?)
        if (!eatCharge(err)) { return false; }
      }
    }
  }

  {
    logstr log;
    log << charges << " charges remain. ";
  }
  return bOK;
} 

bool Obj::isCurrency(ObjEnum otype) {
  switch (otype) {
  case OB_Gold: case OB_Gems: case OB_Emeralds: case OB_Amethysts: return true;
  }
  return false;
}

bool Obj::usesCharges(ObjEnum otype) {
  switch (otype) {
  case OB_Wand: case OB_Staff: return true;
  }
  return false;
}




bool Obj::persist(Persist& p, CPoint& pos) {
  // objdef
  int objdef_ix = Obj::def2ix(objdef); // (p.bOut == output: we output the INDEX of the objdef our data uses.)
  p.transfer(objdef_ix, "objdef_ix");
  if (!p.bOut) { // IE we are reading in.
    objdef = &Obj::objDesc((ObjEnum) objdef_ix); // (p.bOut == input: We locate the ObjDef of the objdef_ix we've read in from file.)
  }

  p.transfer(pos.x, "posx");
  p.transfer(pos.y, "posy");

  p.transfer(ilevel, "ilevel");
  // p.transfer(fakeWeight, "weight");
  p.transfer(itemUnits, "itemunits");
  p.transfer(ac, "ac");
  p.transfer(charges, "charges");
  p.transfer(consumed, "consumed");
  p.transfer(toHit, "tohit");
  p.transfer(toDmg, "todmg");
  p.transfer(dmgDice.n, "dmgDice_n");
  p.transfer(dmgDice.x, "dmgDice_x");

  int int_effect = effect;
  p.transfer(int_effect, "effect"); // FIXME, better handling of enums; possibly we should make them fixedwidth names(?) or at least not numbers.
  p.transfer(soloident, "soloident"); 

  if (!p.bOut) {
    effect = (SpellEnum) int_effect;
  }
  return true;
}


