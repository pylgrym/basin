#include "stdafx.h"
#include "Obj.h"

#include <string>
#include <vector>

#include "util/debstr.h"

std::string Obj::an_item() const {
  CString s = some_item();
  s.Replace(L".", L"a"); // FIXME - a/an for vowels..
  return std::string(CT2A(s));
}

std::string Obj::the_item() const {
  CString s = some_item();
  s.Replace(L".", L"the"); // FIXME - a/an for vowels..
  return std::string(CT2A(s));
}

std::string Obj::indef_item() const {
  CString s = some_item();
  s.Replace(L".", L"");  
  return std::string(CT2A(s));
}


CString Obj::some_item() const {
  const char* sA = typeAsDescA(otype());
  CA2T udesc(sA);
  CString s = udesc;
  CA2T uspell(Spell::type2desc(effect), CP_UTF8);

  const char* aslot = Equ::slotDesc(eqslot);
  if (eqslot == EQ_None) { aslot = "";  } // Don't display 'unwearable', it's superfluous.
  CA2T uslot(aslot, CP_UTF8);

  CString fmt;
  fmt.Format(L"%s of %s %dd%d(%d,%d) %s", (const TCHAR*)s, (const TCHAR*)uspell, dmgDice.n, dmgDice.x, toHit, toDmg, (const TCHAR*) uslot);

  return fmt;
}




ObjDef objDefs[] = {
{OB_None,      EQ_None,    "nothing"}, 
{OB_Lamp,      EQ_None,    ". lamp"}, 
{OB_Sword,     EQ_MainHand,". sword"}, 
{OB_Hat,       EQ_Head,    ". hat"}, 
{OB_Gold,      EQ_None,    ". gold"},
{OB_Potion,    EQ_None,    ". potion"},
{OB_Scroll,    EQ_None,    ". scroll"},
{OB_Staff,     EQ_None,    ". staff"},
{OB_Wand,      EQ_None,    ". wand"},
{OB_Amulet,    EQ_Neck,    ". amulet"},
{OB_Food,      EQ_None,    ". food"},
{OB_Mushroom,  EQ_None,    ". mushroom"},
{OB_Shield,    EQ_OffHand, ". shield"},
{OB_Ring,      EQ_FingerLeft,". ring"}, // FIXME, rings can be worn both left and right.
{OB_Cloak,     EQ_Back,    ". cloak"},
{OB_Water,     EQ_None,    ". water"},
{OB_Bandage,   EQ_None,    ". bandage"},
{OB_Helmet,    EQ_Head,    ". helmet"},
{OB_ChestArmor,EQ_Chest,   ". chest armor"},
{OB_SpellBook, EQ_None,    ". spell book"},
{OB_Key,       EQ_None,    ". key"},
{OB_Candle,    EQ_None,    ". candle"},
{OB_Trap,      EQ_None,    ". trap"},
{OB_StairUp,   EQ_None,    ". stair up"},
{OB_StairDown, EQ_None,    ". stair down"},
{OB_Rune,      EQ_None,    ". rune"},
{OB_Gloves,    EQ_Hands,   ". pair of gloves" },      // Gauntlets/gloves
{OB_Wrists,    EQ_Wrists,  ". pair of bracers"},     // bracers
{OB_Belt,      EQ_Waist,   ". belt"},                 //,=11,      // belt
{OB_Leggings,  EQ_Legs,    ". pair of leggings"},      // = 13,       // Pants/leggings.
{OB_Boots,     EQ_Feet,    ". pair of boots" },           //  = 14,       // Boots/shoes/sandals
};


const TCHAR* Obj::typeAsStr(ObjEnum type) {
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
    thingKeys[OB_Staff] = L"|";
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
    thingKeys[OB_StairUp] = L"stairup";
    thingKeys[OB_StairDown] = L"stairdown";
    thingKeys[OB_Rune] = L"rune";

    // I wish to switch from this to ObjDef system.
    thingKeys[OB_Gloves] = L"glovey";
    thingKeys[OB_Wrists] = L"wristey";
    thingKeys[OB_Belt] = L"beltey";
    thingKeys[OB_Leggings] = L"leggey";
    thingKeys[OB_Boots] = L"bootey";
  }

  if (type < 0 || type >= (int) thingKeys.size()) { return L"out of bounds, thing enum.";  }
  return thingKeys[type];
}


const ObjDef& Obj::objDesc(ObjEnum type) {
  static const ObjDef dummy = { OB_None, EQ_None, "bad obj" };
  const int numObjDefs = (sizeof objDefs / sizeof ObjDef);
  if (type >= 0 && type < numObjDefs) {
    ObjDef* od = &objDefs[type];
    return *od;
  }
  debstr() << "warning/error, bad objEnum?!\n";
  return dummy;
}


const char* Obj::typeAsDescA(ObjEnum type) { // TCHAR*
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
  return itemUnits / 100; // 1500 units will give you 15.
}

void Obj::burnUnits(int units) {
  int remainingUnits = itemUnits - units;
  if (remainingUnits < 0) { remainingUnits = 0;  }
  itemUnits = remainingUnits;
}
