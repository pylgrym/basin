#include "stdafx.h"
#include "Creature.h"

#include <vector>

#include "Mob.h"

bool Creature::isBlockingCreature(CreatureEnum type) {
  switch (type) {
  case CR_None:
    return false; // not blocking.
    // everything else is blocking movement..
  }

  return true;
}



const TCHAR* Creature::typeAsStr(CreatureEnum type) {
  static std::vector<CString> creatureKeys;
  if (creatureKeys.size() == 0) {
    creatureKeys.resize(CR_MaxLimit);
    // creatureKeys[CR_None] = L"notInMap"; 
    creatureKeys[CR_Player] = L"@"; 
    creatureKeys[CR_Kobold] = L"k"; 
    creatureKeys[CR_Dragon] = L"d"; 
  }
  if (type < 0 || type >= (int) creatureKeys.size()) { return L"out of bounds, creature enum.";  }
  return creatureKeys[type];
}


const TCHAR* Creature::ctypeAsDesc(CreatureEnum type) {
  switch (type) {
  case CR_None:   return L". nothing"; 
  case CR_Player: return L"you"; 
  case CR_Kobold: return L". kobold"; 
  case CR_Dragon: return L". dragon"; 
  }
  return L". creature";
}


CreatureEnum Creature::type() const {
  return m != NULL ? m->ctype() : CR_None;
}

