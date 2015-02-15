#include "stdafx.h"
#include "Spell.h"

#include "numutil/myrnd.h"

Spell::Spell()
{
}


Spell::~Spell()
{
}

const SpellDesc Spell::spells[SP_MaxSpells] = {
  { "nospell", "No spell" }, // = 0,
{"speedup", "Haste" }, // = 1,
{"slowdown", "Slow" }, // = 2,
{"confuse", "Confuse" }, // = 3,
{"teleport", "Teleport" }, // = 4,
{"magicmissile", "Magic missile" }, // = 5,
{"firebolt", "Fire bolt" }, // = 6,
{"frostbolt", "Frost bolt" }, // = 7,
{"fireball", "Fire ball" }, // = 8,
{"stonetomud", "Stone to mud" }, // = 9,
{"wallbuilding", "Wallbuilding" }, // = 10
{"stinkcloud", "Stinking cloud" }, // = 11,
};
/* JG: - an idea: These could be auto-assigned to items,
and auto-added to description - "A WAND OF X".
*/


const char* Spell::type2str(SpellEnum type) {
  return spells[type].abbr;
}

const char* Spell::type2desc(SpellEnum type) {
  return spells[type].desc;
}

const SpellDesc& Spell::spell(SpellEnum type) {
  return spells[type];
}



SpellEnum Spell::str2type(const char* str) {
  for (int i = 0; i < SP_MaxSpells; ++i) {
    if (strncmp(str, spells[i].abbr, strlen(str)) == 0) {
      return (SpellEnum) i;
    }
  }

  return SP_NoSpell; // Spellname not matched.
}


SpellEnum Spell::rndSpell() {
  int ix = rnd(SP_MaxSpells);
  return (SpellEnum)ix;
}


bool Spell::doSpell(SpellEnum effect, std::ostream& log) {
  log << "Do:" << effect << ". "; 
  return true; 
}
