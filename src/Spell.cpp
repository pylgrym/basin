#include "stdafx.h"
#include "Spell.h"

#include "numutil/myrnd.h"

#include "Mob.h"

#include "Cmds.h"

Spell::Spell()
{
}


Spell::~Spell()
{
}

const SpellDesc Spell::spells[SP_MaxSpells] = {
{"nospell", "No spell" }, // = 0,
{"speedup", "Haste" }, // = 1,
{"slowdown", "Slow" }, // = 2,
{"confuse", "Confuse" }, // = 3,
{"unconfuse", "Unconfuse" }, // = 3,
{"confusemob", "Confuse monster" }, // = 3,
{"teleport", "Teleport" }, // = 4,
{"magicmissile", "Magic missile" }, // = 5,
{"firebolt", "Fire bolt" }, // = 6,
{"frostbolt", "Frost bolt" }, // = 7,
{"fireball", "Fire ball" }, // = 8,
{"stonetomud", "Stone to mud" }, // = 9,
{"wallbuilding", "Wallbuilding" }, // = 10
{"earthquake", "Earthquake" }, // = 11,
{"stinkcloud", "Stinking cloud" }, // = 12,
{"eat", "Food" }, // = 13,
{"heal", "Healing" }, // = 14,
{"sick", "Sickness" }, // = 15,
{"lightarea", "Light area" }, // = 16,
{"lightbeam", "Light beam" }, // = 17,
};
/* JG: - an idea: These could be auto-assigned to items,
and auto-added to description - "A WAND OF X".
*/


const char* Spell::type2str(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
  return spells[type].abbr;
}

const char* Spell::type2desc(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
  return spells[type].desc;
}

const SpellDesc& Spell::spell(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
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


bool updateSpeed(Mob& actor, double factor) {
  logstr log; if (factor > 1.0) { log << "You speed up."; } else { log << "you slow down."; }
  actor.speed *= factor;
  return true;
}

bool updateConfused(Mob& actor, int confuseCount) {
  logstr log; if (confuseCount > 0) { log << "You feel confused."; } else { log << "You feel less confused."; }
  actor.stats.confused = confuseCount; 
  return true;
}


bool teleportSpell(Mob& actor, int range) {
  { logstr log; log << "Your body shifts in time and space."; }

  for (int i = 0; i < 10; ++i) { // We try a number of times, to avoid teleporting into rock.
    int dx = rndC(-range, range), dy = rndC(-range, range);
    WalkCmd cmd(actor, dx, dy, true);
    // Possibly check 'legal' (for mapPosLegal), even before calling Do.
    logstr log;
    if (cmd.Do(log)) { return true;  } // Otherwise, keep trying different directions.
  }
  return false; // It never worked. Not normal.
}


bool eatSpell(Mob& actor, int deltaFood) {
  {
    logstr log; log << "You eat a bit and feel less hungry!";
    actor.stats.hunger += deltaFood; // Eat 300 something.
  }
  actor.stats.heal(25);
  return true; 
}


bool healSpell(Mob& actor, int percent) {
  {
    logstr log; 
    if (percent > 0) { log << "You feel healing energies."; }
    if (percent < 0) { log << "You feel sick."; }
  }
  actor.stats.heal(percent);
  return true;
}



bool bulletSpell(Mob& actor, AttackSchool school) {
  ZapCmd cmd(actor, school);
  logstr log;
  return cmd.Do(log);
}


bool lightSpell(Mob& actor, CPoint pos, int radius) { 
  // Do a 'light' command:
  actor.lightArea(pos,radius); 
  logstr log;
  log << "Light floods around you.";
  return true;
}



bool Spell::doSpell(SpellEnum effect, Mob& actor, std::ostream& log) { // , Mob* target, 
  log << "Do:" << effect << ". "; 
  switch (effect) {
  case SP_Speedup:      return updateSpeed(actor, 2); break;
  case SP_Slowdown:     return updateSpeed(actor, 0.5); break; 
  case SP_Confuse:      return updateConfused(actor, rnd(3, 20)); break;
  case SP_Unconfuse:    return updateConfused(actor, 0); break;
  case SP_ConfuseMob:   return bulletSpell(actor, SC_Mind); break; // or gas..?
  case SP_Teleport:     return teleportSpell(actor, 4); break;

  case SP_MagicMissile: return bulletSpell(actor, SC_Magic); break; // FIXME - they can have different types of 'missile' for same spell! (school/dmg type?)
  case SP_FireBolt:     return bulletSpell(actor, SC_Fire); break;
  case SP_FrostBolt:    return bulletSpell(actor, SC_Frost); break;
  case SP_FireBall:     return bulletSpell(actor, SC_Fire); break;
  case SP_StinkCloud:   return bulletSpell(actor, SC_Gas); break;

  case SP_StoneToMud:   return bulletSpell(actor, SC_Air); break; // FIXME - a bit different from the other projectile-missile things.
  case SP_WallBuilding: return bulletSpell(actor, SC_Earth); break;
  case SP_Earthquake:   return bulletSpell(actor, SC_Earth); break;

  case SP_Eat:          return eatSpell(actor,300); break;
  case SP_Heal:         return healSpell(actor,35); break;
  case SP_Sick:         return healSpell(actor,-35); break;
  // case SP_Poison:       healSpell(actor); break;
  case SP_LightArea:   return lightSpell(actor, actor.pos,3); break;
  case SP_LightDir:    return lightSpell(actor, actor.pos,3); break; // FIXME, should be zap spell instead..
  default: log << "err spell unknown.";  return false;
  }
  return true; 
}
