#include "stdafx.h"
#include "Spell.h"
#include "numutil/myrnd.h"
#include "Mob.h"
#include "PlayerMob.h"
#include "Cmds.h"
#include "Qual.h"
#include "term.h"
#include <iomanip>
#include "Levelize.h"

extern void playSound(CString soundFile);

/* spell notes: 'light area' appears to be 'critically useful', 
so ability to learn it should happen early,
and/or user should be equippd with a 'helper start' supply
of 'light area' scrolls or similar.
*/

Spell::Spell()
{
}


Spell::~Spell()
{
}

// Format:
// Level, manaCost,(NumDside) minRange, maxRange- semi-random suggestions..
SpellDesc Spell::spells[SP_MaxSpells] = {
{ 1, 1, {1,1}, 0,40,SC_None, "nospell", "No spell" }, // = 0,
{ 2, 2, {1,1}, 0,40,SC_Magic,"speedup", "Haste" }, // = 1,
{ 4, 2, {1,1}, 0,40,SC_Magic,"slowdown", "Slow" }, // = 2,
{ 2, 1, {3,4}, 0,40,SC_Mind,  "confuse", "Confuse self" }, // = 3,
{ 1, 1, {1,1}, 0,40,SC_Mind,  "unconfuse", "Unconfuse" }, // = 4,
{ 2, 1, {3,4}, 0,40,SC_Mind,  "confusemob", "Confuse monster" }, // = 5,
{ 2, 1, {3,4}, 0,40,SC_Mind,  "sleepmob", "Sleep monster" }, // = 5,
{ 5, 2, {1,1 },0,40,SC_Magic, "teleport_me", "Teleport Self" }, // = 6,
{ 6, 3, {1,1 },0,40,SC_Magic, "teleport_you", "Teleport Other" }, // = 6,
// JG, fixme: these powerlevels are arbitrary.
{ 5, 2, { 1, 1 }, 0, 40, SC_Magic, "summonhere", "Summon Here" }, // = 6,
{10, 2, { 1, 1 }, 0, 40, SC_Magic, "summonmob", "Summon Monster" }, // = 6,
{15, 9, { 1, 1 }, 0, 40, SC_Magic, "summonobj", "Summon Item" }, // = 6,
{ 7, 2, { 1, 1 }, 0, 40, SC_Magic, "tele_to", "Phase Near" }, // = 6,
{ 14,4, { 1, 1 }, 0, 10, SC_Magic, "tele_swap", "Swap Places" }, // = 6 // SP_TeleSwap
{ 1, 1, { 2, 4 }, 0, 40, SC_Magic, "magicmissile", "Magic missile" }, // = 7,
{ 6, 2, {3,4}, 0,40,SC_Gas,"stinkcloud", "Stinking cloud" }, // = 14,
{11, 2, {5,6}, 0,40,SC_Fire,"firebolt", "Fire bolt" }, // = 8,
{16, 4, {4,5}, 0,40,SC_Frost,"frostbolt", "Frost bolt" }, // = 9,
{21, 6, {6,7}, 0,40,SC_Fire,"fireball", "Fire ball" }, // = 10,
{26, 2, {7,8}, 0,40,SC_Earth,"stonetomud", "Stone to mud" }, // = 11,
{31, 4, {8,9}, 0,40,SC_Earth,"wallbuilding", "Wallbuilding" }, // = 12
{36, 1,{9,10}, 0,40,SC_Earth,"earthquake", "Earthquake" }, // = 13,
{ 2, 3,{4,6},  2,2,SC_Earth,"focusblast", "Focus Blast" }, // = 13,

{ 2, 3,{4,6},  0,12,SC_Earth,"rush", "Rush" }, // = 13,
{ 4, 3,{4,6},  0,1,SC_Earth,"crush", "Crush" }, // = 13, // Maybe called 'pin/pinning'?
{ 6, 3,{4,6},  0,2,SC_Earth,"embed", "Embed" }, // = 13,
{ 8, 3,{4,6},  0,2,SC_Earth,"shove", "Shove" }, // = 13, // or kick.
{10, 3,{4,6},  0,8,SC_Earth,"tackle", "Tackle" }, // = 13, // maybe longer min-dist req.?

{10, 3,{4,10},  0,25,SC_Fire,"breath", "Breath" }, // = 13, //dragons.. fixme - all schools, not just fire - use mob default school!

{ 9, 2, {1,1}, 0,40,SC_Phys,"eat", "Food" }, // = 15,
{ 1, 1, {1,1}, 0,40,SC_Phys,"heal_light", "Heal light" }, // = 16,
{ 2, 2, {1,1}, 0,40,SC_Phys,"heal_minor", "Heal minor" }, // = 17,
{ 3, 4, {1,1}, 0,40,SC_Phys,"heal_mod", "Heal moderate" }, // = 18,
{ 4, 8, {1,1}, 0,40,SC_Phys,"heal_serious", "Heal serious" }, // = 19,
{ 5,12, {1,1}, 0,40,SC_Phys,"heal_crit", "Healing critical" }, // = 20,
{ 1, 1, {1,1}, 0,40,SC_Phys,"sick", "Sickness" }, // = 21,
{ 1, 2, {1,1}, 0,40,SC_Light,"lightarea", "Light area" }, // = 22, // TODO: I want to make 'light area' easy to learn early/straight away.
{ 5, 3, {1,1}, 0,40,SC_Light,"lightbeam", "Light beam" }, // = 23,
{ 2, 1, {1,1}, 0,40,SC_None,"magicmap", "Magic mapping" }, // = 24,
{ 2, 1, {1,1}, 0,40,SC_None,"phasedoor", "Phase door" }, // = 25,
{ 3, 2, {1,1}, 0,40,SC_None,"detectdoor", "Detect door" },// = 26,
{ 4, 1, {1,1}, 0,40,SC_None,"detecttrap", "Detect trap" }, // = 27,
{ 5, 4, {1,1}, 0,40,SC_None,"detecttreasure", "Detect treasure" }, // = 28,
{ 6, 2, {1,1}, 0,40,SC_None,"detectobject", "Detect object" }, // = 29x,
{ 4, 1, {1,1}, 0,40,SC_None,"detectmobs", "Detect monsters" }, // = 27,
{41, 2, {1,1}, 0,40,SC_None,"restoremana", "Restore Mana" }, // = 29x, // You will NOT be able to learn this..
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

/* JG: - (DONE) an idea: These could be auto-assigned to items,
and auto-added to description - "A WAND OF X".
*/

CPoint Spell::bulletTileForSchool(AttackSchool school) {
  CPoint tileMagic(18, 24);
  CPoint tileFire(22, 24);
  CPoint tileFrost(23, 24);
  CPoint tilePurple(34, 24);
  CPoint tileYellow(35, 24);
  CPoint tileGreenFire(39, 24);
  CPoint tileGreenBall(3, 25);
  CPoint tileLight(39, 2);
  CPoint tileNether(0, 3);
  CPoint tileEarth(16, 22);
  CPoint tileStar(15,22);
  CPoint tileWeird(39, 2);

  CPoint tile = tileMagic;
  switch (school) {
  case SC_Magic: tile = tileMagic; break;
  case SC_Fire:  tile = tileFire;  break;
  case SC_Frost: tile = tileFrost; break;
  case SC_Earth: tile = tileYellow;break;
  case SC_Gas:   tile = tileGreenBall; break;
  case SC_Light: tile = tileLight; break;
  default:       tile = tileWeird; break;
  }
  return tile;
}


const char* Spell::type2str(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
  return spells[type].abbr;
}

const char* Spell::type2desc_Id(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
  return spells[type].desc;
}

const char* Spell::type2desc_Mys(SpellEnum type) {
  assert(type >= 0);
  assert(type < SP_MaxSpells);
  SpellDesc& sd = spells[type];
  if (sd.ident) {
    return sd.desc;
  } else {
    return sd.qual.c_str();
  }
}

const char* Spell::bulletTxt(SpellEnum effect) {
  const SpellDesc& desc = spell(effect);
  return desc.desc; // bullet; // Desc is already a projectile, in context.
}

int Spell::manaCost(SpellEnum effect) {
  const SpellDesc& desc = spell(effect);
  return desc.manaCost; 
}


bool Spell::legalSpellIx(int ix) {
  if (ix < 0) { return false;  }
  if (ix >= SP_MaxSpells) { return false;  }
  return true;
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


SpellEnum Spell::rndSpell_dangerous() {
  int ix = rnd(SP_MaxSpells);
  return (SpellEnum)ix;
}


SpellEnum Spell::rndSpell_level(int ilevel) {
  bool demandDmg = false; // Kludge.
  for (int i = 0; i < 20; ++i) {
    int ix = rnd(SP_MaxSpells);
    SpellEnum stype = (SpellEnum)ix;
    //  make sure we don't get a too powerful blast spell at low level.
    const SpellDesc& d = spell(stype);
    if (d.level <= ilevel && (isDmgSpell(stype) || !demandDmg)) { return stype; }
    if (isDmgSpell(stype)) { demandDmg = true; }
  }
  /* the kludge is about ensuring we get something that does damage,
  if the item level was too high. otherwise, with our default setup,
  damage-wands/staves would be highly unfavoured, because some of them have higher levels.
    The general problem is, that we lack a level-categorizing aspect,
  and general types (ie if we want to keep 'types' balanced,
  then we could start by picking a general category, and then afterwards
  determine a level-appropriate item of that category.
  todo/fixme: same way that I should categorize my monsters, I
  should categorize my items.
  */
  debstr() << "rndSpell_level warning, we gave up..\n";
  return SP_DetectDoor; // Kludge: if we give up, give him a detect-door spell..
}


void Spell::initQual() {
  for (int i = 0; i < SP_MaxSpells; ++i) {
    SpellDesc& desc = spellNC( (SpellEnum) i);
    QualItem& qa = Qual::qual.getItem(i);
    desc.qual = qa.name;
    desc.color = qa.color;
  }
}



bool sleepMob(Mob* target) {
  MonsterMob* monster = dynamic_cast<MonsterMob*>(target);
  if (monster != NULL) {
    monster->mood = M_Sleeping;
    logstr log;
    log << target->pronoun() << " falls to sleep!";
    return true;
  }

  // error - probably a monster trying to sleep the player.
  debstr() << "err, impl player being put to sleep\n";
  logstr log; log << "(fixme) impl. monster putting player to sleep.\n";
  return false;
}



class SpellImpl {
public:
  virtual bool getParams(SpellParam& param) { return true;  }
  virtual bool execSpell(SpellParam& param) = 0;
};



bool updateSpeed(Mob& actor, double factor) {
  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  logstr log; if (factor > 1.0) { log << "You speed up."; } else { log << "you slow down."; }
  actor.speed *= factor;
  return true;
}

class Spell_Speed : public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return updateSpeed(*param.actor, param.factor);  }
  static void init(Mob& actor, double factor, SpellParam& p) { p.actor = &actor;  p.factor = factor; p.impl = &spell_speed; }
} spell_speed;







bool teleportSpell(Mob& actor, int range) {
  playSound(L"sounds\\sfxr\\teleport.wav"); // teleport-spell cast.

  { logstr log; log << "Your body shifts in time and space."; }

  for (int i = 0; i < 10; ++i) { // We try a number of times, to avoid teleporting into rock.
    CPoint delta;
    for (;;) {
      delta.x = rndC(-range, range); delta.y = rndC(-range, range);
      CPoint newPos = actor.pos + delta;
      if (CL->map.legalPos(newPos)) { break; }
    }
    WalkCmd cmd(actor, delta.x, delta.y, true);
    // Possibly check 'legal' (for mapPosLegal), even before calling Do.
    logstr log;
    if (cmd.Do(log)) { return true;  } // Otherwise, keep trying different directions.
  }
  return false; // It never worked. Not normal.
}

class Spell_Tele : public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return teleportSpell(*param.actor, param.range);  }
  static void init(Mob& actor, int range, SpellParam& p) { p.actor = &actor;  p.range = range; p.impl = &spell_tele; }
} spell_tele;


bool updateConfused(Mob& actor, int confuseCount) {
  if (confuseCount > 0) { // If counter is set positive, a confusion-spell has been cast on someone.
    playSound(L"sounds\\sfxr\\confuse.wav"); // confusion-spell cast.
  }

  logstr log; if (confuseCount > 0) { log << actor.pronoun() << " feel" << actor.verbS() << " confused."; } else { log << actor.pronoun() << " feel" << actor.verbS() << " less confused."; }
  actor.stats.confused = confuseCount; 
  return true;
}

class Spell_Confuse : public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return updateConfused(*param.actor, param.confuse);  }
  static void init(Mob& actor, int confuse, SpellParam& p) { p.actor = &actor;  p.confuse = confuse; p.impl = &spell_confuse; }
} spell_confuse;


bool eatSpell(Mob& actor, int deltaFood) {
  {
    logstr log; log << "You eat a bit and feel less hungry!";
    actor.stats.hunger += deltaFood; // Eat 300 something.
  }
  actor.stats.healPct(25, &actor);
  return true; 
}

class Spell_Eat: public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return eatSpell(*param.actor, param.deltaFood);  }
  static void init(Mob& actor, int deltaFood, SpellParam& p) { p.actor = &actor;  p.deltaFood = deltaFood; p.impl = &spell_eat; }
} spell_eat;


bool healSpellPct(Mob& actor, int percent) {
  {
    logstr log; 
    if (percent > 0) { 
      log << actor.pronoun() << " feel" << actor.verbS()<< " healing energies."; 
    }
    if (percent < 0) { 
      log << actor.pronoun() << " feel" << actor.verbS() << " sick."; 
    }
  }
  actor.stats.healPct(percent, &actor);
  return true;
}

class Spell_HealPct: public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return healSpellPct(*param.actor, param.healPct);  }
  static void init(Mob& actor, int healPct, SpellParam& p) { p.actor = &actor;  p.healPct = healPct; p.impl = &spell_healPct; }
} spell_healPct;


bool healSpellDice(Mob& actor, Dice dice) {
  std::stringstream dummy;
  int val = dice.roll(dummy);
  {
    logstr log;
    if (val > 0) { log << actor.pronoun() << " feel" << actor.verbS() << " healing energies."; }
    if (val < 0) { log << actor.pronoun() << " feel" << actor.verbS() << " sick."; }
  }
  actor.stats.healAbs(val, &actor);
  return true;
}

class Spell_HealDice: public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return healSpellDice(*param.actor, param.healDice);  }
  static void init(Mob& actor, Dice healDice, SpellParam& p) { p.actor = &actor;  p.healDice = healDice; p.impl = &spell_healDice; }
} spell_healDice;



bool manaSpellPct(Mob& actor, int percent) {
  {
    logstr log; 
    if (percent > 0) { 
      log << actor.pronoun() << " feel" << actor.verbS()<< " mana flowing through 'you'."; 
    }
    if (percent < 0) { 
      log << actor.pronoun() << " feel" << actor.verbS() << " mana draining away from 'you'."; 
    }
  }
  actor.stats.manaPct(percent, &actor);
  return true;
}

class Spell_ManaPct: public SpellImpl { 
public:
  bool execSpell(SpellParam& param) { return manaSpellPct(*param.actor, param.manaPct);  }
  static void init(Mob& actor, int manaPct, SpellParam& p) { p.actor = &actor;  p.manaPct = manaPct; p.impl = &spell_manaPct; }
} spell_manaPct;



// Consider: 'school' should not be extra arg to bulletSpell, because spell itself knows magicschool.
bool bulletSpell(Mob& actor, Obj* spellItem, SpellEnum effect, AttackSchool school, CPoint dir) { // error/fixme: spell itself already knows school, so specifying it twice leads to ambiguity redundancy errors!
  ZapCmd cmd(spellItem, actor, effect, school);
  cmd.mobZapDir = dir;
  logstr log;
  return cmd.Do(log);
}

class Spell_Bullet: public SpellImpl { 
public:
  bool getParams(SpellParam& param) {
    return getParamsDIR(param);
  }

  static bool getParamsDIR(SpellParam& param) { 
    // If it's a mob, it will handle dir by itself.
    if (param.actor->isPlayer()) {
      param.dir = Spell::pickZapDir();
      if (param.dir == Spell::NoDir) { return false;  }
    } else {
      if (param.target != NULL) {
        CPoint deltaDir = (param.target->pos - param.actor->pos);
        param.dir = Mob::normDir(deltaDir);
      }
    }
    return true; 
  }

  bool execSpell(SpellParam& param) { return bulletSpell(*param.actor, param.item, param.effect, param.school, param.dir);  }
  static void init(Mob& actor, Obj* item, SpellEnum effect, AttackSchool school, SpellParam& p) { 
    p.actor = &actor;  p.item = item; p.effect = effect; p.school = school; p.impl = &spell_bullet; 
  }
} spell_bullet;


struct CheckCellBase { 
  virtual bool check(const Cell&) = 0; 
  virtual std::string what() = 0;
};

struct IsObj  : public CheckCellBase { virtual bool check(const Cell& c) { return !c.item.empty(); }      std::string what(){return "items";} };
struct IsMob  : public CheckCellBase { virtual bool check(const Cell& c) { return !c.creature.empty(); }  std::string what(){return "monsters";} };
struct IsDoor : public CheckCellBase { virtual bool check(const Cell& c) { return c.envir.isDoor() || c.envir.isStair(); }    std::string what(){return "doors or stairs";} };
struct IsTrap : public CheckCellBase { virtual bool check(const Cell& c) { return (c.item.type() == OB_Trap); }   std::string what(){return "traps";} };


struct IsTreasure : public CheckCellBase {
  virtual bool check(const Cell& c) {
    return Obj::isCurrency(c.item.type());
    // switch (c.item.type()) {
    // case OB_Gold: return true;
    // case OB_Emeralds: return true;
    // }
    // return false;
  }
  std::string what(){return "treasure";}
};


bool detectCells(Mob& actor, CPoint pos, int radius, CheckCellBase& checker) {
  int count = 0;
  for (int dx = -radius; dx <= radius; ++dx) {
    for (int dy = -radius; dy <= radius; ++dy) {
      CPoint p(pos.x + dx, pos.y + dy);
      if (CL->map.legalPos(p)) {
        Cell& c = CL->map[p];
        if (checker.check(c)) {
          c.lightCells(p); //FIXME, you could argue we only want a temp highlight.
          c.envir.tmpLightStr = 0; // this was the reason detect-x didn't work: the weird lighting stuff made distant cells blackish.
          // Also, this highlight follows the cell, not the item/feature..
          TheUI::invalidateCell(p); // JG - there is a bug here - though we do the invalidate, we don't get our redraw? do we need to specify 'erase'?
          ++count;
        }
        // if (c.envir.blocked()) {}
      }
    }
  }
  if (count == 0) {
    logstr log; log << actor.pronoun() << " don't sense" << actor.verbS() << " anything nearby."; // FIXME, 'doesn't'.
  } else {
    // Invalidate / cuss:: (it's ok, invalidateCell above does this.)
    //Cuss::invalidate(); // FIXME, is this the right way?
    Cuss::invalidate(); // hack, because we are not getting our expected redraw?!
    logstr log; log << actor.pronoun() << " sense" << actor.verbS() << " " << checker.what() << " nearby!";
  }
  return true;
}

bool detectDoors(CPoint pos, int radius,Mob& actor) { return detectCells(actor,pos, radius, IsDoor()); } //, CheckCellBase& checker) {
bool detectTraps(CPoint pos, int radius,Mob& actor) { return detectCells(actor,pos, radius, IsTrap()); } //, CheckCellBase& checker) {
bool detectTreasure(CPoint pos, int radius,Mob& actor) { return detectCells(actor,pos, radius, IsTreasure()); } //, CheckCellBase& checker) {
bool detectObj(CPoint pos, int radius,Mob& actor) { return detectCells(actor,pos, radius, IsObj()); } //, CheckCellBase& checker) {
bool detectMobs(CPoint pos, int radius,Mob& actor) { return detectCells(actor,pos, radius, IsMob()); } //, CheckCellBase& checker) {

bool spellDetect(Mob& actor, SpellEnum effect) {
  CPoint pos = actor.pos;
  int rad = 23;
  switch (effect) {
  case SP_DetectDoor:    return detectDoors(pos, rad,actor);
  case SP_DetectTrap:    return detectTraps(pos, rad,actor);
  case SP_DetectTreasure:return detectTreasure(pos, rad,actor);
  case SP_DetectObject:  return detectObj(pos, rad,actor);
  case SP_DetectMobs:    return detectMobs(pos, rad,actor);
  }
  assert(false);  // spellDetect shouldn't happen.
  return false;
}


bool lightSpell(Mob& actor, CPoint pos, int radius) {
  /* idea: that light-area sets s light-strength on the tiles.
  */
  // Do a 'light' command:
  actor.lightArea(pos,radius); 
  logstr log;
  log << "Light floods around you.";
  return true;
}

class Spell_Light: public SpellImpl { 
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return lightSpell(*param.actor, param.pos, param.radius);  }
  static void init(Mob& actor, CPoint pos, int radius, SpellParam& p) { p.actor = &actor;  p.pos = pos; p.radius = radius;  p.impl = &spell_light; }
} spell_light;


bool summonSpell(Mob& actor) { // , CPoint pos, int radius) {
  // CONSIDER: Summon-Mob-Type, e.g. undead, demon, dragon, elemental, etc.
  CPoint pos = actor.pos;
  int mlevel = rndC(1,2) + Levelize::suggestLevel(actor.stats.level()); // Scary - a bit higher than we'd like :-)
  CL->map.scatterMobsAtPos(pos, 1, mlevel, 1);
  // actor.lightArea(pos, radius);
  logstr log;
  log << "A monster shimmers before you!";
  return true;
}

class Spell_SummonMob : public SpellImpl {
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return summonSpell(*param.actor); }
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_summonMob; }
} spell_summonMob;

bool summonObj(Mob& actor) { // , int count) { // , CPoint pos, int radius) {
  CPoint pos = actor.pos;
  int ilevel = Levelize::suggestLevel(actor.stats.level());
  CL->map.scatterObjsAtPos(pos, rndC(1,2), ilevel, 1);
  // actor.lightArea(pos, radius);
  logstr log;
  log << "Items shimmer before you!";
  return true;
}

class Spell_SummonObj : public SpellImpl {
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return summonObj(*param.actor); }
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_summonObj; }
} spell_summonObj;


bool teleportTo(Mob& actor, CPoint targetpos, Mob* aim) {
  CL->map.moveMob(actor, targetpos);
  if (actor.isPlayer()) { actor.lightWalls(); }  // Fixme - moving always needs this? (we don't want move+light everytime.)

  logstr log;
  log << "The air shimmers!";
  return true;
}

class Spell_TeleTo : public SpellImpl {
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return teleportTo(*param.actor, param.pos, param.target); }
  static void init(Mob& actor, CPoint targetpos, Mob* aim, SpellParam& p) { p.actor = &actor;  p.pos = targetpos; p.target = aim;  p.impl = &spell_teleTo; }
} spell_teleTo;


/*
idea: 'standing in bad' dots - leaving temp bad tiles on ground, that you must avoid standing on or possibly next to.

 - these are similar - you and/or mob must slide like a projectile.
  SP_Rush,   // charge - you launch YOURSELF as a projectile, and bump into the mob. so - both projectile and 'teleport' (works similarly to teleport-to)
  SP_Shove,  // push mob sliding along the floor, to bumb into far wall.
  SP_Tackle, // push both yourself and mob sliding along the floor, bumping into wall. it's 'more of rush'.

  - semi-done:
  SP_Crush,  // crush mob between hard wall and yourself (requires a wall on the other side.) (doesn't require moving anyone, just a 'spell melee')
  SP_Embed,  // Push mob INTO the rock.. (do you move along with it?) (requires moving at least the mob. MOVE: DONE.. - rock-dmg NOT done)
*/


bool spellRush(Mob& actor, CPoint dir) {
  // NB! if you do this without hitting a mob, you'll hurt yourself badly for 2/3 of your (remaining) health!
  // (I want to discourage using it for plain moving/fast moving..)
  // Actually, it could still be used to flee fast along a corridor, to get quickly away from a mob (at the price of 2/3 hp..)

  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  { logstr log; log << "You would rush headfirst into the mob..";  }
  // Todo: projectile + move player.. Make a toolbox to build spells..

  // Todo: this loop could be put in an iteratorish-class:
  CPoint newPos = actor.pos;
  Cell* cell = NULL;
  for (;;) {
    newPos += dir;
    cell = &CL->map[newPos];
    if (cell->blocked()) { break; }
    actor.moveM(newPos);
    TheUI::microSleepForRedraw(7); // animate, fast - let us see mob moving.
  }

  if (!cell->creature.empty()) { // Good: we bump into an enemy!
    { logstr log; log << "You bump into the mob!"; }
    const bool doOverrideHit = true;
    HitCmd rush(NULL,actor,dir.x, dir.y,SC_Phys,SP_Rush, doOverrideHit); // FIXME; how much dmg does it do, and does it stun/confuse him?
    logstr log;
    return rush.Do(log);
  }

  if (cell->envir.blocked()) {
    if (actor.isPlayer()) { logstr log; log << "Augh! You rush into the wall, seriously hurting yourself!"; }
    else { logstr log; log << "Ouch! The monster slams into the wall, seriously hurting itself!"; }
    
    healSpellPct(actor, -66); // Loose 66 pct of health!
    return true;
  }
  logstr log; log << "Weird, your rush leads nowhere?";
  return true;
}



class Spell_Rush: public SpellImpl { public:
  bool getParams(SpellParam& param) { return Spell_Bullet::getParamsDIR(param); }
  bool execSpell(SpellParam& param) { return spellRush(*param.actor, param.dir);  } // Consider: we could impl directly here!
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_rush; }
} spell_rush;



bool spellCrush(Mob& actor, Mob& target, CPoint dir) {
  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  { logstr log; log << "You would pin the mob against the wall.."; }
  // NB! Must check mob is adj, and wall on other side.. Make a toolbox to build spells..
  // Consider: maybe it's HitCmd instead?
  // Todo: target ought to be passed along to hit/zap cmd..
  ZapCmd cmd(NULL, actor, SP_Crush, SC_Phys); // school);
  cmd.mobZapDir = dir;
  logstr log;
  return cmd.Do(log);
}


class Spell_Crush: public SpellImpl { public:
  bool getParams(SpellParam& p) { return getParamsCRUSH(p);  }

  static bool getParamsCRUSH(SpellParam& p) { 
    bool paramOK = Spell_Bullet::getParamsDIR(p); 
    if (!paramOK) { return false;  }
    p.pos = p.actor->pos + p.dir;
    p.target = CL->map[p.pos].creature.m;
    if (p.target == NULL) { if (p.actor->isPlayer()) { logstr log; log << "But there is noone there to crush?"; } return false; }
    if (!CL->map[p.pos+p.dir].envir.blocked()) { if (p.actor->isPlayer()) { logstr log; log << "But there is no wall to crush against?"; } return false; }
    return true;
  }
  bool execSpell(SpellParam& param) { return spellCrush(*param.actor, *param.target, param.dir);  } // Consider: we could impl directly here!
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_crush; }
} spell_crush;



bool spellEmbed(Mob& actor, Mob& target, CPoint dir) {
  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  { logstr log; log << "You attempt to embed the mob in the rock.."; }
  // Todo: must check mob is adj, and wall on other side.. Make a toolbox to build spells..

  // (50%done)-FIXME - this does not push mob into wall, and move you..
  CPoint embedPos = (target.pos + dir);
  target.moveM(embedPos); //CL->map.moveMob(target, target.pos);

  ZapCmd cmd(NULL, actor, SP_Embed, SC_Phys); // school);
  cmd.mobZapDir = dir;
  logstr log;
  return cmd.Do(log);
}


class Spell_Embed: public SpellImpl { public:
  bool getParams(SpellParam& param) { // Embed has same requirements as Crush (wall on other side, and mob in between, directly next to you.)
    return Spell_Crush::getParamsCRUSH(param); 
  }
  bool execSpell(SpellParam& param) { return spellEmbed(*param.actor, *param.target, param.dir);  } // Consider: we could impl directly here!
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_embed; }
} spell_embed;


class Spell_Detect: public SpellImpl { public:
  bool execSpell(SpellParam& param) { return spellDetect(*param.actor, param.effect);  } // Consider: we could impl directly here!
  static void init(Mob& actor, SpellEnum effect, SpellParam& p) { p.actor = &actor; p.effect = effect; p.impl = &spell_detect; }
} spell_detect;


bool spellShove(Mob& actor, Mob& target, CPoint dir) {
  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  { logstr log; log << "You would shove the mob along the ground.."; }
  // Todo: must check mob is adj, and wall on other side.. Make a toolbox to build spells..
  // was: - this does not push mob into wall, and move you..

  return spellRush(target, dir); // hack: move mob instead.. // fixme.. hitting the MOB for 66% of health is too extreme.

  /*
  ZapCmd cmd(NULL, actor, SP_Shove, SC_Phys); // school);
  cmd.mobZapDir = dir;
  logstr log;
  return cmd.Do(log);
  */
}

/* thoughts: i should consider making 'floodfill-laby' prettier.

i should make restore mana
*/

class Spell_Shove: public SpellImpl { public:
  bool getParams(SpellParam& param) { return getParamsSHOVE(param); }

  static bool getParamsSHOVE(SpellParam& p) { 
    bool paramOK = Spell_Bullet::getParamsDIR(p); 
    if (!paramOK) { return false;  }
    p.pos = p.actor->pos + p.dir;
    p.target = CL->map[p.pos].creature.m;
    if (p.target == NULL) { if (p.actor->isPlayer()) { logstr log; log << "But there is noone there to shove?"; } return false; }
    // if (!CL->map[p.pos+p.dir].envir.blocked()) { if (p.actor->isPlayer()) { logstr log; log << "But there is no wall to crush against?"; } return false; }
    return true;
  }

  bool execSpell(SpellParam& param) { return spellShove(*param.actor, *param.target, param.dir);  } // Consider: we could impl directly here!
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_shove; }
} spell_shove;



bool spellTackle(Mob& actor, Mob& target, CPoint dir) {
  /* fixme - the physical spells don't seem to either identify or eat-charges correctly - 'shove' never eats its charges?
  */

  // fiXmE - is 'target anything yet? (it might be,  because we assume both are next to each other? but actually, 'tackle' may use run-lead-up?
  playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
  { logstr log; log << "You would tackle the mob.."; }
  // Todo: must check mob is adj, and wall on other side.. Make a toolbox to build spells..

  // First hurl-part, only actor:
  CPoint newActorPos = actor.pos;
  Cell* cell = NULL;
  for (;;) {
    newActorPos += dir;
    cell = &CL->map[newActorPos];
    if (cell->blocked()) { break; }
    actor.moveM(newActorPos);
    TheUI::microSleepForRedraw(7); // animate, fast - let us see mob moving.
  }

  if (cell->envir.blocked()) {
    if (actor.isPlayer()) { logstr log; log << "Augh! You slam into the wall, seriously hurting yourself!"; }
    else { logstr log; log << "Ouch! The monster slams into the wall, seriously hurting itself!"; }    
    healSpellPct(actor, -66); // Loose 66 pct of health!
    return true;
  }

  if (cell->creature.empty()) { // bad unexpected we didn't bump into an enemy?
    { logstr log; log << "Why is there noone here?"; }
    return true;
  }
  Mob* mob = cell->creature.m;

  // Second stage, now both mob and actor must slide along.
  { logstr log; log << "You tackle the mob!"; }

  CPoint newMobPos = newActorPos+dir;
  for (;;) {
    newActorPos += dir;
    newMobPos += dir;
    cell = &CL->map[newMobPos];
    if (cell->blocked()) { break; }
    mob->moveM(newMobPos);
    actor.moveM(newActorPos);
    TheUI::microSleepForRedraw(7); // animate, fast - let us see mob moving.
  }

  { // this is a bit cheating - tackle, ie mob hitting wall with me, gets dmg by me hitting mob with 'tackle..'
    const bool overrideHit = true;
    HitCmd tackle(NULL,actor,dir.x, dir.y,SC_Phys,SP_Tackle,overrideHit); // FIXME; how much dmg does it do, and does it stun/confuse him?
    logstr log;
    return tackle.Do(log);
  }

  logstr log; log << "Weird, your tackle leads nowhere?";

  /*
  ZapCmd cmd(NULL, actor, SP_Tackle, SC_Phys); // school);
  cmd.mobZapDir = dir;
  logstr log;
  return cmd.Do(log);
  */
}

class Spell_Tackle: public SpellImpl { public:
  bool getParams(SpellParam& param) { return getParamsTACKLE(param); }

  static bool getParamsTACKLE(SpellParam& p) { 
    bool paramOK = Spell_Bullet::getParamsDIR(p); 
    if (!paramOK) { return false;  }
    p.pos = p.actor->pos + p.dir;
    p.target = CL->map[p.pos].creature.m;
    // FIXME - tackle doesn't work this way! it's more akin to rush!
    if (p.target == NULL) { if (p.actor->isPlayer()) { logstr log; log << "But there is noone there to tackle?"; } return false; }
    // if (!CL->map[p.pos+p.dir].envir.blocked()) { if (p.actor->isPlayer()) { logstr log; log << "But there is no wall to crush against?"; } return false; }
    return true;
  }

  bool execSpell(SpellParam& param) { return spellTackle(*param.actor, *param.target, param.dir);  } // Consider: we could impl directly here!
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_tackle; }
} spell_tackle;


bool Spell::manaCostCheck(SpellEnum effect, Mob& mob, std::ostream& err) {
  if (mob.stats.mana >= Spell::manaCost(effect)) {
    return true; 
  }

  // DONE: make a general prompter helper func, that integrates all this:
  LogEvents::respectMultiNotif(); // Pause if we have queued messages, before prompting.
  Cuss::clear(false);
  const char* keyPrompt = "Your mana is exhausted. Risk it Y/N?";
  int YN_Key = 0;
  bool bFound = false;
  for (;!bFound;) {
    YN_Key = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "risk cast,Y/N"); 

    if (YN_Key == VK_ESCAPE || YN_Key == 'N') {
      Cuss::clear(true);
      return false; // Cancelled zap operation.
    }
    if (YN_Key == 'Y') {bFound = true;  break;}
  } // Loop until Y/N/Esc key.

  bool bFail = oneIn(3); 
  if (!bFail) {
    logstr log; log << "You pull it off! ..";
    return true;
  }

  err << "You fumble and damage your health.";
  int severity = rnd(25, 50);
  debstr() << "hurt-severity-pct:" << severity << "\n";
  mob.stats.healPct(-severity, &mob);
  return false;
}



// bool Spell::execSpell(SpellParam& p) { //, SpellEnum effect, Mob& actor, Mob* target, std::ostream& log, Obj* item, const ManaEnum useMana) {  
//  p.impl->execSpell(p);
//}

  /* FIXME: I need to clarify, the 'sender/receiver' - actor/victim - aspect of all this:
   - you can cast these on others, or on yourself.
   (the difference between you casting confusion on someone else, or on yourself
    -and them casting it on you.)
  */


///////////////////////////////////////////////////////NB, 'target' here not thought through!
bool Spell::prepareSpell(SpellParam& p, SpellEnum effect, Mob& actor, Mob* target, Obj* item) {  
  // prepareSpell fills out a SpellParam! - makes sure e.g. bullet-aim-dir is known
  switch (effect) {
  case SP_Speedup:          Spell_Speed::init(actor, 2, p);       break; // return updateSpeed(actor, 2); break;
  case SP_Slowdown:         Spell_Speed::init(actor, 0.5, p);     break; //return updateSpeed(actor, 0.5); break; 

  case SP_ConfuseSelf:    Spell_Confuse::init(actor,rnd(5,25),p); break; //return updateConfused(actor, p.confuse); break; // Careful, 'confuse' is the 'recipient part'
  case SP_Unconfuse:      Spell_Confuse::init(actor, 0, p);       break; //   return updateConfused(actor, 0); break;         // Careful, 'confuse' is the 'recipient part'
  case SP_ConfuseMob:     Spell_Bullet::init(actor, item, effect, SC_Mind, p); break; // return bulletSpell(actor, item, effect, SC_Mind); break; // or gas..? // Conversely, this is the 'sender part' // It should actually just use 'confuseself' for bullet. (very fitting, how the confuse-spell has worked for the programmer himself.)

    // sleep other, here?
  case SP_SleepOther:      Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_TeleSelfAway:      Spell_Tele::init(actor, 44, p);      break; //return teleportSpell(actor, 44); break; // (Consider: We need a 'bullet teleport' too) This is only the 'receiver' part.
  case SP_TeleOtherAway:   Spell_Bullet::init(actor, item, SP_TeleSelfAway, SC_Mind, p); break; // return bulletSpell(actor, item, SP_TeleSelfAway, SC_Mind); break;


  case SP_SummonHere:      Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // x goes to me.
  case SP_SummonMonster:Spell_SummonMob::init(actor, p); break; //return summonSpell(actor); break;
  case SP_SummonObj:    Spell_SummonObj::init(actor, p); break; //return summonObj(actor); break;
  // it's a bullet spell, so it goes here:
  case SP_TeleportTo:     Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // i go to mob x.
  case SP_TeleSwap:       Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // i swap with x.

  case SP_MagicMissile:   Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_FireBolt:       Spell_Bullet::init(actor, item, effect, SC_Fire, p); break; // return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_FrostBolt:      Spell_Bullet::init(actor, item, effect, SC_Frost,p); break; // return bulletSpell(actor, item, effect, SC_Frost); break;
  case SP_FireBall:       Spell_Bullet::init(actor, item, effect, SC_Fire, p); break; // return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_StinkCloud:     Spell_Bullet::init(actor, item, effect, SC_Gas, p);  break; // return bulletSpell(actor, item, effect, SC_Gas); break;
  case SP_StoneToMud:     Spell_Bullet::init(actor, item, effect, SC_Air, p);  break; // return bulletSpell(actor, item, effect, SC_Air); break;  // error/fixme: spell already knows school, so specifying it twice leads to ambiguity redundancy errors!
  case SP_WallBuilding:   Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;
  case SP_Earthquake:     Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;
  case SP_FocusBlast:     Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;

  case SP_Rush:             Spell_Rush::init(actor, p);                        break; 
  case SP_Crush:           Spell_Crush::init(actor, p);                        break; 
  case SP_Embed:           Spell_Embed::init(actor, p);                        break; 
  case SP_Shove:           Spell_Shove::init(actor, p);                        break; 
  case SP_Tackle:         Spell_Tackle::init(actor, p);                        break; 

                       // FiXME, dragon-breath is NOT tackle!
  case SP_Breath:         Spell_Tackle::init(actor, p);                        break; 

  case SP_Eat:         Spell_Eat::init(actor, item ? item->itemUnits : 250, p);break; // return eatSpell(actor,p.deltaFood); break;
  case SP_Sick:        Spell_HealPct::init(actor, -35, p);                     break; // return healSpellPct(actor,p.healPct); break;
  // Heal dice design: lots of small dice, so you always get some healing.
  case SP_Heal_light:   Spell_HealDice::init(actor, Dice(4,3), p);             break; //return healSpellDice(actor, p.healDice); break;
  case SP_Heal_minor:   Spell_HealDice::init(actor, Dice(6,3), p);             break; //return healSpellDice(actor, Dice(6,  3)); break;
  case SP_Heal_mod:     Spell_HealDice::init(actor, Dice(8,3), p);             break; //return healSpellDice(actor, Dice(8,  3)); break;
  case SP_Heal_serious: Spell_HealDice::init(actor, Dice(10,3), p);            break; //return healSpellDice(actor, Dice(10, 3)); break;
  case SP_Heal_crit:    Spell_HealDice::init(actor, Dice(12,3), p);            break; //return healSpellDice(actor, Dice(12, 3)); break;
  case SP_LightArea:       Spell_Light::init(actor, actor.pos,4, p);           break; //return lightSpell(actor, actor.pos,4); break;
  case SP_LightDir:       Spell_Bullet::init(actor, item, effect, SC_Light, p);break; //return bulletSpell(actor, item, effect, SC_Light); break; // actor.pos, 3); break; // FIXME, should be zap spell instead.. (sure?)

  case SP_DetectDoor: case SP_DetectTrap: case SP_DetectTreasure: case SP_DetectObject: case SP_DetectMobs:
                          Spell_Detect::init(actor, effect,p);                 break;

  case SP_MagicMap:        Spell_Light::init(actor, actor.pos,22, p);          break; //return lightSpell(actor, actor.pos,4); break;
  // case SP_MagicMap: { logstr log;  log << "(magicmap not impl yet.)"; } return false;
  case SP_PhaseDoor:        Spell_Tele::init(actor, 44, p);      break; //return teleportSpell(actor, 9); break;

    // Idea -it could be partial with blanks/particles?
    /* todo:
void detectObj(CPoint pos, int radius) { return detectCells(pos, radius, IsObj()); } //, CheckCellBase& checker) {
void detectMobs(CPoint pos, int radius) { return detectCells(pos, radius, IsMob()); } //, CheckCellBase& checker) {
void detectDoors(CPoint pos, int radius) { return detectCells(pos, radius, IsDoor()); } //, CheckCellBase& checker) {
void detectTraps(CPoint pos, int radius) { return detectCells(pos, radius, IsTrap()); } //, CheckCellBase& checker) {
void detectTreasure(CPoint pos, int radius) { return detectCells(pos, radius, IsTreasure()); } //, CheckCellBase& checker) {
    */

  case SP_RestoreMana:   Spell_ManaPct::init(actor, 33, p);                    break; // return healSpellPct(actor,p.healPct); break;
  // case SP_Poison:       healSpell(actor); break;
  default: { logstr log; log << "err spell unknown:" << effect; } return false;
  }

  if (p.impl == NULL) { return false; }  // we've only prepared a spell, if we've located an 'impl'. 
  bool bPromptOK = p.impl->getParams(p); // e.g. prompt for dir.
  return bPromptOK; 
}


bool SpellParam::exec() {
  assert(impl != NULL);
  bool xOK = impl->execSpell(*this);
  return xOK; 
}

bool Spell::castSpell(SpellEnum spellType, Mob& actor, Mob* target, Obj* item, const ManaEnum useMana) {  
  /* castSpell combines preparespell, manacheck, manaeat, and actual execute.
  */

  /* I've introduced 'spellparams'. -it adds multi-switch.
  order is 
  1 - choose spell.
  2 - collect complete params.(dir) (handled here in prepareSpell) 
  3 - check mana is available or risked.
  4 - consume mana
  5 - execute spell.  
  */

  SpellParam param; // collect 'full info', ie 'dir' for bullet spell.
  bool collectOK = Spell::prepareSpell(param, spellType, actor, target, item); // in CastCmd::Do.
  if (!collectOK) { return false; }

  if (useMana == UseMana) { // 3: User-self-cast spells must cost him mana.
    // I prefer to postpone this check to AFTER user has specified any direction
    // (So we don't do the check multiple times, if he reconsiders.)
    logstr log;
    bool suffMana = Spell::manaCostCheck(spellType, actor, log); // 3, check enough mana.
    if (!suffMana) { return false; }

    // 4 - eat mana - At this point, we eat the mana:
    // NB!- zap/dir commands should only consume mana AFTER dir is specified.
    int manaCost = Spell::manaCost(spellType);
    if (!actor.stats.useMana(manaCost)) {
      logstr log; log << "useMana failure!";
      return false;
    }    
  }

  // 5 - execute spell:
  bool castOK = param.exec();
  return castOK;
}



CPoint Spell::pickZapDir() {
  bool bFound = false;
  int dirKey = 0;

  // FIXME - respectMultiNotif and promptForKey should be integrated!
  LogEvents::respectMultiNotif(); // Pause if we have queued messages, before prompting.
  Cuss::clear(false); // Must come after respectMultiNotif, or we'll never see msg.
  const char* keyPrompt = "Zap which direction?";
  for (;!bFound;) {
    dirKey = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-zap-dir"); 

    if (dirKey == VK_ESCAPE) {
      Cuss::clear(true);
      return NoDir; // false; // Cancelled zap operation.
    }
    
    switch (dirKey) {
    case 'H': case 'J': case 'K': case 'L': case 'N': case 'B': case 'U': case 'Y': bFound = true;  break;
    default: TheUI::BeepWarn(); break; // Not a DIR key.
    }      
  } // Loop until dir key.

  CPoint dir = Map::key2dir(dirKey); 
  return dir;
}

CPoint Spell::NoDir = CPoint(0,0); // 0,0


// (DONE) Make doors!

void Spell::trySpellIdent(SpellEnum effect) {
  SpellDesc& desc = spellNC(effect);
  if (desc.ident) { debstr() << "(already known)\n"; return;  }
  if (oneIn(2)) { // Maybe every use should identify automatically.. 
    logstr log; log << "you get a sense of what this item does!";
    // NB; Selling should ID it! 
    // DONE NB!  - spells should cost differently! 
    // DONE NB, spells should be divided by levels, 
    // also monsters, and monsters should have some way to use obj/spells..?
    desc.ident = true; 
  }
}


/* player should start with MagicMissile,
using a book should learn a spell.
  Maybe you can temporarily lose/forget a spell, and have to relearn it?
*/

SpellEnum Spell::pickASpell(const char* prompt) {

  int offset = 0;
  while (true) {
    Cuss::clear(false);
    Cuss::prtL(prompt);
    showSpellInv(offset, SpellPageSize); 

    bool cancel = false;
    SpellEnum choice = pickSpellAction(offset, cancel);
    if (cancel) { return SP_NoSpell; }
    if (choice != SP_NoSpell) { return choice; }
    // Otherwise, it's just offset-browse-list
  }
}



void Spell::showSpellInv(int offset, int numItems) { 
  /* FIXME; how about more than one page?
  */

  char ix = 'a';

  // Put column headers in front:
  std::stringstream ss1;
  ss1 << "K  L I A M Desc (key,level,ident.,learned,cost)";
  Cuss::prtL(ss1.str().c_str());  

  int knowncount = 0;
  int count = 0;
  for (int i = SP_NoSpell+1 + offset; i < SP_MaxSpells && count < numItems; ++i, ++ix, ++count) {
    if (Cuss::csr.y >= Term::Height-2) { break;  } // i > Term::Height - 2) { break;  } // FIXME, stay on screen.

    SpellDesc& d = spellNC( (SpellEnum) i); 
    SpellEnum checkType = (SpellEnum)i;

    std::stringstream ss;
    ss << ix;
    ss << " " << std::fixed << std::setw(2) << d.level; // << "L";
    ss << " " << (d.ident ? "!" : "?"); // << "I";
    ss << " " << (d.ability ? "+" : "-"); // << "A";
    ss << " " << d.manaCost; // << "M";
    ss << " " << d.desc << "";

    COLORREF rowColor = RGB(255, 255, 255);
    rowColor = (d.ability ? RGB(0, 255, 0) : RGB(100,100,100) ); // Green is known, grey is unknown, red is too high.
    if (d.level > PlayerMob::ply->stats.level()) { rowColor = RGB(175, 0, 0);  }
    Cuss::setTxtColor(rowColor);
    
    Cuss::prtL(ss.str().c_str());  

    knowncount += (d.ability);
  }
  Cuss::setTxtColor(); // Reset to default

  if (knowncount == 0) {
    Cuss::prtL(" (You don't know any of these spells.)"); // Nothing.Nada."); 
  }
  /* Idea:terminal could use half-width chars,
  so that every 32x32 tile, would contain 2 vertical letters.
  This would allow me to get '80 chars', and enable 'much better text'.
    I lack page-up.
  */

  Cuss::invalidate();
}




SpellEnum Spell::pickSpellAction(int& offset, bool& cancel) { 
  int lastSpellIx = offset + SpellPageSize;
  if (lastSpellIx >= SP_MaxSpells) { lastSpellIx = SP_MaxSpells - 1; }
  int firstSpellIx = offset + (SP_NoSpell + 1);
  int curCount = (lastSpellIx - firstSpellIx) +1;

  const char firstKey = 'A';
  char lastKey = firstKey + (curCount-1); //  SpellPageSize; // 'Z'; // firstKey + objs.size() - 1; // Bag::bag.
  char lower = lastKey - ('A' - 'a');

  int curPage = (offset / SpellPageSize) + 1;
  // (35 should divide to 7 (with 5), but 36 up to 40 should give 8 (and 41 again 9.)
  int totPage = ((SpellPageSize-1)+(1+(SP_MaxSpells-1)-(SP_NoSpell+1))) / SpellPageSize;

  CString s; 
  s.Format(L"(%d/%d)(letter [a-%c] or ESC)", curPage,totPage,lower);
  CT2A keyPrompt(s, CP_ACP);  

  int key = 0;
  for (;;) {
    key = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-item"); 
    if (key == VK_ESCAPE) {
      cancel = true;
      Cuss::clear(true);
      return SP_NoSpell; // Cancelled pick operation.
    }
    // self-cast/zap. - not always a dir?
    // Make new spell command.

    // spell pagination: // FIXME, isolate pagination mechanism, and share between display and choice-picker.
    int oldOffset = offset;
    if (key == VK_SPACE || key == VK_NEXT) { offset += SpellPageSize;  }
    if (key == VK_PRIOR) { offset -= SpellPageSize;  }
    if (offset < 0) { offset = 0; }
    if (offset >= SP_MaxSpells) { offset = 0; }
    if (offset != oldOffset) { return SP_NoSpell;  } // redisplay paginated list.
    // end spell-pagination.

    if (key >= firstKey && key <= lastKey) {
      break;
    }
    TheUI::BeepWarn();
  }

  int objIx = key - firstKey;

  int spellIx = SP_NoSpell + 1 + offset + objIx;
  SpellEnum spellChoice = (SpellEnum)spellIx;
  if (!Spell::legalSpellIx(spellChoice)) {
    logstr log; log << "Illegal spell choice:" << spellIx; return SP_NoSpell;
  }

  const SpellDesc& desc = Spell::spell(spellChoice);
  if (!desc.ability) {
    logstr log; log << "But you don't know that spell!"; return SP_NoSpell;
  }

  debstr() << "You have picked:" << spellIx << "\n";

  Cuss::clear(true);
  return spellChoice;
}




bool Spell::persist(class Persist& p) {

  int spellCount = SP_MaxSpells - SP_NoSpell; // As NoSpell == 0, spellcount becomes equal to MaxSpell, and is correct loop limit.
  p.transfer(spellCount, "spellCount");

  for (int i = SP_NoSpell+1; i < spellCount; ++i) {

    int spellType =  i; // When outbound, this is in control.
    p.transfer(spellType, "spellType_ix");
    SpellDesc& desc = spellNC( (SpellEnum) spellType); // Inbound, spellType comes from spellType_ix.

    p.transfer(desc.ident, "ident");
    p.transfer(desc.ability, "ability");
  }

  return true;
}



bool Spell::isDmgSpell(SpellEnum stype) {
  switch (stype) {
  case SP_MagicMissile:  // SC_Magic
  case SP_StinkCloud:   // SC_Poison/SC_Gas
  case SP_FireBolt:    // SC_Fire
  case SP_FrostBolt:    // SC_Frost
  case SP_FireBall:     // SC_Fire  10
  case SP_StoneToMud:   // SC_Air/Elm? // "DONE"
  case SP_WallBuilding: // SC_Earth    // "DONE"
  case SP_Earthquake:  // SC_Earth    // Todo - different kind of spell! (it's like a 'mass digging'.)
  case SP_FocusBlast:  // SC_Earth    // Todo - different kind of spell! (it's like a 'mass digging'.)
    return true;
  }
  return false;
}

