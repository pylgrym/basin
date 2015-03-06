#include "stdafx.h"
#include "Spell.h"

#include "numutil/myrnd.h"

#include "Mob.h"

#include "Cmds.h"
#include "Qual.h"


extern void playSound(CString soundFile);

Spell::Spell()
{
}


Spell::~Spell()
{
}

SpellDesc Spell::spells[SP_MaxSpells] = {
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
{"heal_light", "Heal light" }, // = 14,
{"heal_minor", "Heal minor" }, // = 14,
{"heal_mod", "Heal moderate" }, // = 14,
{"heal_serious", "Heal serious" }, // = 14,
{"heal_crit", "Healing critical" }, // = 14,
{"sick", "Sickness" }, // = 15,
{"lightarea", "Light area" }, // = 16,
{"lightbeam", "Light beam" }, // = 17,
{"phasedoor", "Phase door" }, // = 15,
{"detectdoor", "Detect door" },// = 15,
{"detecttrap", "Detect trap" }, // = 15,
{"detecttreasure", "Detect treasure" }, // = 15,
{"detectobject", "Detect object" }, // = 15,
/*
{"detectmagic",""}, // 
{"light",""}, // 
{"burninghands",""}, // 
{"charmanimal",""}, // 
{"charmperson",""}, // 
{"curelightwounds",""}, // 
{"curemoderatewounds",""}, // 
{"cureseriouswounds",""}, // 
{"curecriticalwounds",""}, // 
{"detectdoors",""}, // 
{"magicmissile",""}, // 
{"darkness",""}, // 
{"dazemonster",""}, // 
{"delaypoison",""}, // 
{"ghoultouch",""}, // 
{"invisibility",""}, // 
{"levitate",""}, // 
{"acidarrow",""}, // 
{"shatter",""}, // 
{"silence",""}, // 
{"summonmonster",""}, // 
{"web",""}, // 
{"polymorph",""}, // 
{"lightning",""}, // 
{"dispelmagic",""}, // 
{"searinglight",""}, // 
{"fear",""}, // 
{"icestorm",""}, // 
{"curepoison",""}, // 
{"poison",""}, // 
{"walloffire",""}, // 
{"chaoshammer",""}, // 
{"holysmite",""}, // 
{"orderswrath",""}, // 
{"unholyblight",""}, // 
{"restoration",""}, // 
{"stoneskin",""}, // http://www.d20srd.org/srd/magicItems/wands.htm

charming
fire
swarminginsects
healing
illumination
frost
defense
abjuration
conjuration
enchantment
evocation
illusion
necromancy
life
passage
power
summonswarm, cloudkill,shieldoflaw
locateobjects
locatetreasure
locatedoors
sensemonsters
sensesurroundings
glimpsesurroundings
sleep, mass x.
coneofcold
removedisease
cureblindness
sunburst
haltundead
circleofdeath
passwall
phasedoor
teleport
dimensiondoor
invulnerable
freeaction
light
stayinglight
blink
disintegrate
barkskin   http://www.d20srd.org/srd/magicItems/staffs.htm
wallofthorns http://www.d20srd.org/srd/magicItems/potionsAndOils.htm
removefear
bless
sanctuary/rune/ward
blindness/seeing
featherfall
trap
curse/removecurse
rage
calm
neutralize-cure-poison
heroism,haste,speed.
flamearrow,daylight.
removeparalysis
*/
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

const char* Spell::type2descB(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
  SpellDesc& sd = spells[type];
  if (sd.ident) {
    return sd.desc;
  } else {
    return sd.qual.c_str();
  }
}

const SpellDesc& Spell::spell(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);

  static bool bInit = false;
  if (!bInit) {
    bInit = true;
    initQual();
  }
  return spells[type];
}

SpellDesc& Spell::spellNC(SpellEnum type) {
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


void Spell::initQual() {
  for (int i = 0; i < SP_MaxSpells; ++i) {
    SpellDesc& desc = spellNC( (SpellEnum) i);
    QualItem& qa = Qual::qual.getItem(i);
    desc.qual = qa.name;
    desc.color = qa.color;
  }
}

bool updateSpeed(Mob& actor, double factor) {
  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  logstr log; if (factor > 1.0) { log << "You speed up."; } else { log << "you slow down."; }
  actor.speed *= factor;
  return true;
}

bool updateConfused(Mob& actor, int confuseCount) {
  if (confuseCount > 0) { // If counter is set positive, a confusion-spell has been cast on someone.
    playSound(L"sounds\\sfxr\\confuse.wav"); // confusion-spell cast.
  }

  logstr log; if (confuseCount > 0) { log << "You feel confused."; } else { log << "You feel less confused."; }
  actor.stats.confused = confuseCount; 
  return true;
}


bool teleportSpell(Mob& actor, int range) {
  playSound(L"sounds\\sfxr\\teleport.wav"); // teleport-spell cast.

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
  actor.stats.healPct(25);
  return true; 
}


bool healSpellPct(Mob& actor, int percent) {
  {
    logstr log; 
    if (percent > 0) { log << "You feel healing energies."; }
    if (percent < 0) { log << "You feel sick."; }
  }
  actor.stats.healPct(percent);
  return true;
}

bool healSpellDice(Mob& actor, Dice dice) {
  std::stringstream dummy;
  int val = dice.roll(dummy);
  {
    logstr log;
    if (val > 0) { log << "You feel healing energies."; }
    if (val < 0) { log << "You feel sick."; }
  }
  actor.stats.healAbs(val);
  return true;
}



bool bulletSpell(Mob& actor, Obj* spellItem, SpellEnum effect, AttackSchool school) {
  ZapCmd cmd(spellItem, actor, effect, school);
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



bool Spell::doSpell(SpellEnum effect, Mob& actor, std::ostream& log, Obj* item) {  

  switch (effect) {
  case SP_Speedup:      return updateSpeed(actor, 2); break;
  case SP_Slowdown:     return updateSpeed(actor, 0.5); break; 
  case SP_Confuse:      return updateConfused(actor, rnd(5, 25)); break;
  case SP_Unconfuse:    return updateConfused(actor, 0); break;
  case SP_ConfuseMob:   return bulletSpell(actor, item, effect, SC_Mind); break; // or gas..?
  case SP_Teleport:     return teleportSpell(actor, 44); break;
  case SP_PhaseDoor:     return teleportSpell(actor, 9); break;

  case SP_MagicMissile: return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_FireBolt:     return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_FrostBolt:    return bulletSpell(actor, item, effect, SC_Frost); break;
  case SP_FireBall:     return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_StinkCloud:   return bulletSpell(actor, item, effect, SC_Gas); break;

  case SP_StoneToMud:   return bulletSpell(actor, item, effect, SC_Air); break;  
  case SP_WallBuilding: return bulletSpell(actor, item, effect, SC_Earth); break;
  case SP_Earthquake:   return bulletSpell(actor, item, effect, SC_Earth); break;

  case SP_Eat:          return eatSpell(actor,item->itemUnits); break;

  // case SP_Heal:         return healSpellPct(actor,35); break;
  case SP_Sick:         return healSpellPct(actor,-35); break;

  // Heal dice design: lots of small dice, so you always get some healing.
  case SP_Heal_light:   return healSpellDice(actor, Dice(4,  3)); break;
  case SP_Heal_minor:   return healSpellDice(actor, Dice(6,  3)); break;
  case SP_Heal_mod:     return healSpellDice(actor, Dice(8,  3)); break;
  case SP_Heal_serious: return healSpellDice(actor, Dice(10, 3)); break;
  case SP_Heal_crit:    return healSpellDice(actor, Dice(12, 3)); break;

  // case SP_Poison:       healSpell(actor); break;
  case SP_LightArea:   return lightSpell(actor, actor.pos,4); break;
  case SP_LightDir:    return bulletSpell(actor, item, effect, SC_Light); break; // actor.pos, 3); break; // FIXME, should be zap spell instead..
  default: log << "err spell unknown.";  return false;
  }
  return true; 
}


void Spell::trySpellIdent(SpellEnum effect) {
  SpellDesc& desc = spellNC(effect);
  if (desc.ident) { debstr() << "(already known)\n"; return;  }
  if (oneIn(2)) { // Maybe every use should identify automatically.. 
    logstr log; log << "you get a sense of what this item does!";
    // NB; Selling should ID it! 
    // NB!  - spells should cost differently! 
    // NB, spells should be divided by levels, also monsters, and monsters should have some way to use obj/spells..?
    desc.ident = true; 
  }
}
