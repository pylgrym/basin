#include "stdafx.h"
#include "Equ.h"
#include "numutil/myrnd.h"
#include "Obj.h"
#include "Bag.h"
#include "Cuss.h"
#include "theUI.h"
#include <iomanip>



Equ Equ::worn;

Equ::~Equ() {}

EquipSlotEnum Equ::rndSlot() {
  int val = rnd::Rnd(0, EQ_MaxSlot);
  EquipSlotEnum slot = (EquipSlotEnum)val;
  return slot;
}

bool Equ::isSlotEquipped(EquipSlotEnum slot) const { // Used by e.g. isWeaponEquipped and canUnequipSlot and unequipWeaponSlot.
  return (equ[slot] != NULL);  
}



bool Equ::isWeaponEquipped(EquipSlotEnum newslot) const {
  if (isSlotEquipped(EQ_2Hands)) { return true; } // If a 2hander is equipped, it blocks any weapon.
  if (isSlotEquipped(newslot)) { return true;  } // If same slot is equipped, it obviously blocks.

  if (newslot == EQ_2Hands) { // If we want to equip a 2hander, any weapon slot blocks.
    if (isSlotEquipped(EQ_MainHand)) { return true; }
    if (isSlotEquipped(EQ_OffHand)) { return true; }
  }
  return false; // Nothing blocking.
}

// Stash is arg to unequipWeaponSlot and unequipSlot and replaceItem.
// Stash seems to happen outside / only arg to here.

bool Equ::unequipWeaponSlot(EquipSlotEnum newslot, Obj** objStash, std::ostream& err) {
  //bool Equ::unequipSlot(EquipSlotEnum slot, Obj** objStash, std::ostream& err) { // returns false if not possible (if e.g. worn is cursed.)

  if (isSlotEquipped(EQ_2Hands)) { return unequipSlot(EQ_2Hands, objStash, err); } // If a 2hander is equipped, it blocks any weapon.

  if (isSlotEquipped(newslot)) { return unequipSlot(newslot, objStash, err);  } // If same slot is equipped, it obviously blocks.

  bool bOK = false;

  if (newslot == EQ_2Hands) { // If we want to equip a 2hander, any weapon slot blocks.
    if (isSlotEquipped(EQ_MainHand)) { 
      if (!Bag::bag.canAdd()) { err << "Bag is full.";  return false; }
      if (!unequipSlot(EQ_MainHand, objStash, err)) { return false;  }      
      bOK = Bag::bag.add(*objStash, err); // unequipped things must go in bag. 
      *objStash = NULL;
    }
    if (isSlotEquipped(EQ_OffHand)) { 
      if (!Bag::bag.canAdd()) { err << "Bag is full.";  return false; }
      if (!unequipSlot(EQ_OffHand, objStash, err)) { return false;  }
      bOK = Bag::bag.add(*objStash, err); // unequipped things must go in bag.
      *objStash = NULL;
    }
    return true;
  }
  return false; // Nothing to do - shouldn't happen.
}



bool Equ::canUnequipSlot(EquipSlotEnum slot) const { 
  if (!isSlotEquipped(slot)) { // You can't unequip, if nothing is equipped.
    return false; 
  }
  return true; // we don't have cursed-items yet.
} 


bool Equ::canEquipSlot(EquipSlotEnum slot) const { 
  if (!isSlotEquipped(slot)) { return true;  } // If nothing is equipped, you are free to equip item.
  return canUnequipSlot(slot); // IF whatever is equipped can be unequipped, you can equip something else.
} // means 'same' thing as 'canUnequipSlot': (you can only equip, IF you can unequip previous.)


Obj* Equ::wornItem(EquipSlotEnum slot) const { // Used by weapon().
  return equ[slot];  
}


Obj* Equ::weapon() const {
  if (isSlotEquipped(EQ_2Hands)) { return wornItem(EQ_2Hands);  }
  if (isSlotEquipped(EQ_MainHand)) { return wornItem(EQ_MainHand);  }
  if (isSlotEquipped(EQ_OffHand)) { return wornItem(EQ_OffHand);  }
  return NULL; // No weapon equipped.
}


bool Equ::canEquipItem(Obj* item) const { return true; } // FIXME: You really need the class/player info to determine this.


bool Equ::unequipSlot(EquipSlotEnum slot, Obj** objStash, std::ostream& err) { // returns false if not possible (if e.g. worn is cursed.)
  // You get the object returned, that you unequip (you must put it in your bag or on the floor.)
  if (!isSlotEquipped(slot)) { err << "You don't have anything to unequip."; return false; }
  if (!canUnequipSlot(slot)) { err << "You can't unequip that.";  return false; }
  *objStash = equ[slot];
  equ[slot] = NULL;
  return true; 
}

bool Equ::equipItem(Obj* item, std::ostream& err) { // returns false if not possible (if e.g. worn is cursed.)
  if (isSlotEquipped(item->eqslot())) { err << "You must unequip first."; return false; }
  equ[item->eqslot()] = item;
  return true;
} 

bool Equ::unequipItem(Obj* item, std::ostream& err) { // returns false if not possible (if e.g. worn is cursed.)
  // Not really used, currently.
  if (!canUnequipSlot(item->eqslot())) { err << "You can't unequip that."; return false; }
  equ[item->eqslot()] = NULL;
  return true;
}

bool Equ::replaceItem(Obj* newItem, Obj** objStash, std::ostream& err) { // combines unequipitem and equipItem.
  // Beware that objStash now is messy: for weapons, we return null, for non-weapons, we return the actual swapped-out object :-(.

  // Hack/kludge: weapons, because they 'share hands', are messy:
  if (isWeapon(newItem->eqslot())) {
    if (isWeaponEquipped(newItem->eqslot())) {
      logstr log;  log << "You unequip the old weapon.";
      if (!unequipWeaponSlot(newItem->eqslot(), objStash, err)) {
        return false;
      }
    }
  } else { // Not a weapon:
    if (isSlotEquipped(newItem->eqslot())) {
      if (!unequipSlot(newItem->eqslot(), objStash, err)) {
        return false;
      }
      logstr log;  log << "You unequip the old "; if (*objStash) { log << (**objStash).indef_item();  } else { log << " item."; }
    }
  }

  logstr log;  log << "You equip "; log << newItem->the_item();
  bool equipOK = equipItem(newItem, err);
  return equipOK;
}



const char* Equ::slotDesc(EquipSlotEnum slot) {
  const char* descs[] = {
    "Unwearable",
    "Weapon(MH)",
    "Weapon(OH)",
    "Weapon(2H)",
    "Head",
    "Neck",
    "Chest",
    "Hands",
    "Wrists",
    "Finger(R)",
    "Finger(L)",
    "Waist",
    "Back",
    "Legs",
    "Feet",
  };

  if (slot < 0 || slot > EQ_MaxSlot) { return "ErrIx"; }
  return descs[slot];
}


bool Equ::isWeapon(EquipSlotEnum slot) {
  switch (slot) {
  case EQ_MainHand: case EQ_OffHand: case EQ_2Hands: return true;
  }
  return false;
}



int Equ::calcWornAC() {
  int totalAC = 0;
  for (int i = EQ_Unwearable + 1; i <= EQ_MaxSlot; ++i) {
    EquipSlotEnum slot = (EquipSlotEnum)i;
    Obj* o = equ[slot];
    if (o != NULL) {
      totalAC += o->ac;
    }
  }
  return totalAC;
}


double Equ::wornWeight() {
  double totalWeight = 0;
  for (int i = EQ_Unwearable + 1; i <= EQ_MaxSlot; ++i) {
    EquipSlotEnum slot = (EquipSlotEnum)i;
    Obj* o = equ[slot];
    if (o != NULL) {
      totalWeight += o->kweight();
    }
  }
  return totalWeight;
}


void Equ::showWorn() {
  if (equ.size() == 0) {
    Cuss::prtL("Zero items worn."); // Nothing.Nada."); 
  }

  char ix = 'a';
  for (int i = EQ_Unwearable + 1; i <= EQ_MaxSlot; ++i, ++ix) {
    EquipSlotEnum slot = (EquipSlotEnum) i;
    Obj* o = equ[slot];

    std::string descA; std::string label;
    double weight = 0.0;
    if (o != NULL) {
      descA = o->an_item();
      weight = o->kweight();
      label = ""; // std::string(slotDesc(slot)) + ":";
    } else  {
      descA = " - ";
    }


    std::stringstream ss;
    ss << ix << " " << std::fixed << std::setprecision(1);
    if (o) { ss << weight; } else  { ss << " .";  }
    ss << " " << label << descA;
    Cuss::prtL(ss.str().c_str());  
  }

  double wornTotal = wornWeight();
  double everything = Bag::bag.bagWeight() + wornTotal;
  std::stringstream ss;
  ss << "Total weight: " << wornTotal << " kg";
  ss << " (" << everything << ")";
  Cuss::prtL(ss.str().c_str());


  Cuss::invalidate();

}



int Equ::wornCount() const {
  int count = 0;
  for (int i = EQ_Unwearable + 1; i <= EQ_MaxSlot; ++i) {
    if (equ[i] != NULL) { ++count;  }
  }
  return count;
}



Obj* Equ::pick(const char* prompt) {
  Cuss::clear(false);
  Cuss::prtL(prompt); 

  showWorn(); 
  const char firstKey = 'A';
  char lastKey = firstKey + (equ.size()-1)-1;  // because first item is not used.
  char lower = lastKey - ('A' - 'a');
  CString s; 
  s.Format(L"(letter [a-%c] or ESC)", lower);
  CT2A keyPrompt(s, CP_ACP);  

  int objIx = -1;
  int key = 0;
  for (;;) {
    key = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "equip-pick"); 
    if (key == VK_ESCAPE) {
      Cuss::clear(true);
      return NULL; // Cancelled pick operation.
    }
    objIx = (key - firstKey)+1; // offset by one - we don't use EQ_Unwearable.
    if (key >= firstKey && key <= lastKey && equ[objIx] != NULL) {
      break;
    }
    TheUI::BeepWarn();
  }

  Obj* obj = equ[objIx];

  debstr() << "You picked:" << obj << "\n";

  Cuss::clear(true);
  return obj;
}


bool Equ::persist(class Persist& p) {
  int equCount = wornCount();
  p.transfer(equCount, "wornCount");

  if (p.bOut) { // if outputting.
    for (int i = EQ_Unwearable + 1; i <= EQ_MaxSlot; ++i) {
      if (equ[i] != NULL) {
        p.transfer(i, "eqSlot");
        CPoint unused;
        equ[i]->persist(p, unused);
      }
    } // worn-item-loop.
  } else { // if reading.
    const ObjDef& dummy = Obj::objDesc(OB_None);
    for (int i = 0; i < equCount; ++i) {
      int eqSlot = 0;
      p.transfer(eqSlot, "eqSlot");
      Obj* o = new Obj(dummy, 1);
      CPoint unused;
      o->persist(p, unused);
      equ[eqSlot] = o;
    } // worn-item-loop.
  }

  return true;
}

