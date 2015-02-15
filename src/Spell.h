#pragma once

#include <ostream>

/* JG: Spell/Spellnum allows me to map enums to text-tags and back.
This makes it possible for me to have sets of spell effects on items.

  Right now, relation between Commands and Spells is not totally resolved;
they are cousins.. Commands are, sort of, spells,
and some commands will trigger spells.
  A meeting/overlap, is that the 'Use' command, on an item,
will trigger the spell effect of the item.
  Note, that spells, like commands, may have arbitrary 'owner' and 'victim/target'
(both player and mob.)
*/

enum SpellEnum {
  SP_NoSpell = 0,
  SP_Speedup = 1,
  SP_Slowdown = 2,
  SP_Confuse = 3,
  SP_Teleport = 4,
  SP_MagicMissile = 5,
  SP_FireBolt = 6,
  SP_FrostBolt = 7,
  SP_FireBall = 8,
  SP_StoneToMud = 9,
  SP_WallBuilding = 10,
  SP_StinkCloud = 11,
  SP_MaxSpells = 12
};

struct SpellDesc {
  const char* abbr;
  const char* desc;
};

class Spell
{
public:
  Spell();
  ~Spell();
  static const char* type2str(SpellEnum);
  static const char* type2desc(SpellEnum);
  static SpellEnum str2type(const char* str);
  static const SpellDesc& spell(SpellEnum);
  static SpellEnum rndSpell();

  static bool doSpell(SpellEnum, std::ostream& log);

private:
  static const SpellDesc spells[SP_MaxSpells];
};

