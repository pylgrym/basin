#pragma once

#include <vector>

enum EquipSlotEnum {
  EQ_Unwearable=0,EQ_None=0,
  EQ_MainHand=1,
  EQ_OffHand=2,
  EQ_2Hands=3,
  EQ_Head=4,       // Helmet/hats
  EQ_Neck=5,       // amulet
  EQ_Chest=6,      // Breastplate/armour
  EQ_Hands=7,      // Gauntlets/gloves
  EQ_Wrists=8,     // bracers
  EQ_FingerLeft=9, // ring
  EQ_FingerRight=10,// ring
  EQ_Waist=11,      // belt
  EQ_Back=12,       // Cloaks/drapes/wraps.
  EQ_Legs=13,       // Pants/leggings.
  EQ_Feet=14,       // Boots/shoes/sandals
  EQ_MaxSlot=14 // same as last slot.
};

class Obj; 

class Equ
{
public:
  Equ() { equ.resize(EQ_MaxSlot+1);  }
  std::vector<Obj*> equ;
  bool replaceItem(Obj* item, Obj** objStash, std::ostream& err); // combines unequipitem and equipItem.
  bool equipItem(Obj* item, std::ostream& err); // returns false if not possible (if e.g. worn is cursed.)
  bool unequipItem(Obj* item, std::ostream& err); // returns false if not possible (if e.g. worn is cursed.)
  bool unequipSlot(EquipSlotEnum slot, Obj** objStash, std::ostream& err);
  bool unequipWeaponSlot(EquipSlotEnum newslot, Obj** objStash, std::ostream& err);

  // Rule: 'inspectors' don't report errors, only 'actions' report errors.

  Obj* weapon() const; 
  Obj* wornItem(EquipSlotEnum slot) const; // { return equ[slot];  }
  bool isSlotEquipped(EquipSlotEnum slot) const; // { return (equ[slot] != NULL);  }
  bool isWeaponEquipped(EquipSlotEnum slot) const; 
  bool canUnequipSlot(EquipSlotEnum slot) const; // { return true; } // we don't have cursed-items yet.
  bool canEquipSlot(EquipSlotEnum slot) const; // { return canUnequipSlot(slot); } // means same thing. (you can only equip, IF you can unequip previous.)
  bool canEquipItem(Obj* item) const; // { return true; } // FIXME: You really need the class/player info to determine this.

  int calcWornAC();

  void showWorn();
  Obj* pick(const char* prompt);

  double wornWeight();

  int wornCount() const;

  /* map slot to 'slot desc  - " on hands, chest, head, neck:
  */
 
  // TODO/FIXME: a further check for 'canEquip', is 'can this char/player/class/mob equip this (heavy-heavy, skill-demanding) item?
  ~Equ();

  bool persist(class Persist& p);

  static Equ worn;
  static EquipSlotEnum rndSlot();

  static const char* slotDesc(EquipSlotEnum slot);
  static bool isWeapon(EquipSlotEnum slot);


};

