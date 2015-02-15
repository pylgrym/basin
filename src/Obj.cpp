#include "stdafx.h"
#include "Obj.h"

#include <string>
#include <vector>

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
  CString s = typeAsDesc(otype());
  CA2T uspell(Spell::type2desc(effect), CP_UTF8);
  CA2T uslot(Equ::slotDesc(eqslot), CP_UTF8);

  CString fmt;
  fmt.Format(L"%s of %s %dd%d(%d,%d) %s", (const TCHAR*)s, (const TCHAR*)uspell, dmgDice.n, dmgDice.x, toHit, toDmg, (const TCHAR*) uslot);

  return fmt;
}



const TCHAR* Obj::typeAsStr(ObjEnum type) {
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

  }
  if (type < 0 || type >= (int) thingKeys.size()) { return L"out of bounds, thing enum.";  }
  return thingKeys[type];
}


const TCHAR* Obj::typeAsDesc(ObjEnum type) {
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
  return L"obj error";
}


ObjEnum ObjSlot::type() const {
  return o != NULL ? o->otype() : OB_None;
}



bool Obj::wearable() const {
  return (eqslot != EQ_Unwearable); // Equippable);
}
