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

{ 9, 2, {1,1}, 0,40,SC_Phys,"eat", "Food" }, // = 15,
{ 1, 1, {1,1}, 0,40,SC_Phys,"heal_light", "Heal light" }, // = 16,
{ 2, 2, {1,1}, 0,40,SC_Phys,"heal_minor", "Heal minor" }, // = 17,
{ 3, 4, {1,1}, 0,40,SC_Phys,"heal_mod", "Heal moderate" }, // = 18,
{ 4, 8, {1,1}, 0,40,SC_Phys,"heal_serious", "Heal serious" }, // = 19,
{ 5,12, {1,1}, 0,40,SC_Phys,"heal_crit", "Healing critical" }, // = 20,
{ 1, 1, {1,1}, 0,40,SC_Phys,"sick", "Sickness" }, // = 21,
{ 3, 2, {1,1}, 0,40,SC_Light,"lightarea", "Light area" }, // = 22,
{ 5, 3, {1,1}, 0,40,SC_Light,"lightbeam", "Light beam" }, // = 23,
{ 2, 1, {1,1}, 0,40,SC_None,"magicmap", "Magic mapping" }, // = 24,
{ 2, 1, {1,1}, 0,40,SC_None,"phasedoor", "Phase door" }, // = 25,
{ 3, 2, {1,1}, 0,40,SC_None,"detectdoor", "Detect door" },// = 26,
{ 4, 1, {1,1}, 0,40,SC_None,"detecttrap", "Detect trap" }, // = 27,
{ 5, 4, {1,1}, 0,40,SC_None,"detecttreasure", "Detect treasure" }, // = 28,
{ 6, 2, {1,1}, 0,40,SC_None,"detectobject", "Detect object" }, // = 29x,
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
  //bool getParams(SpellParam& param) { param.factor = 1.0;  return true; }
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
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return teleportSpell(*param.actor, param.range);  }
  static void init(Mob& actor, int range, SpellParam& p) { p.actor = &actor;  p.range = range; p.impl = &spell_tele; }
} spell_tele;


bool updateConfused(Mob& actor, int confuseCount) {
  if (confuseCount > 0) { // If counter is set positive, a confusion-spell has been cast on someone.
    playSound(L"sounds\\sfxr\\confuse.wav"); // confusion-spell cast.
  }

  logstr log; if (confuseCount > 0) { log << "You feel confused."; } else { log << "You feel less confused."; }
  actor.stats.confused = confuseCount; 
  return true;
}

class Spell_Confuse : public SpellImpl { 
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return updateConfused(*param.actor, param.confuse);  }
  static void init(Mob& actor, int confuse, SpellParam& p) { p.actor = &actor;  p.confuse = confuse; p.impl = &spell_confuse; }
} spell_confuse;


bool eatSpell(Mob& actor, int deltaFood) {
  {
    logstr log; log << "You eat a bit and feel less hungry!";
    actor.stats.hunger += deltaFood; // Eat 300 something.
  }
  actor.stats.healPct(25);
  return true; 
}

class Spell_Eat: public SpellImpl { 
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return eatSpell(*param.actor, param.deltaFood);  }
  static void init(Mob& actor, int deltaFood, SpellParam& p) { p.actor = &actor;  p.deltaFood = deltaFood; p.impl = &spell_eat; }
} spell_eat;


bool healSpellPct(Mob& actor, int percent) {
  {
    logstr log; 
    if (percent > 0) { log << "You feel healing energies."; }
    if (percent < 0) { log << "You feel sick."; }
  }
  actor.stats.healPct(percent);
  return true;
}

class Spell_HealPct: public SpellImpl { 
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return healSpellPct(*param.actor, param.healPct);  }
  static void init(Mob& actor, int healPct, SpellParam& p) { p.actor = &actor;  p.healPct = healPct; p.impl = &spell_healPct; }
} spell_healPct;


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

class Spell_HealDice: public SpellImpl { 
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return healSpellDice(*param.actor, param.healDice);  }
  static void init(Mob& actor, Dice healDice, SpellParam& p) { p.actor = &actor;  p.healDice = healDice; p.impl = &spell_healDice; }
} spell_healDice;


// Consider: 'school' should not be extra arg to bulletSpell, because spell itself knows magicschool.
bool bulletSpell(Mob& actor, Obj* spellItem, SpellEnum effect, AttackSchool school) { // error/fixme: spell itself already knows school, so specifying it twice leads to ambiguity redundancy errors!
  ZapCmd cmd(spellItem, actor, effect, school);
  logstr log;
  return cmd.Do(log);
}

class Spell_Bullet: public SpellImpl { 
public:
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return bulletSpell(*param.actor, param.item, param.effect, param.school);  }
  static void init(Mob& actor, Obj* item, SpellEnum effect, AttackSchool school, SpellParam& p) { p.actor = &actor;  p.item = item; p.effect = effect; p.school = school; p.impl = &spell_bullet; }
} spell_bullet;



bool lightSpell(Mob& actor, CPoint pos, int radius) { 
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
  logstr log;
  log << "The air shimmers!";
  return true;
}

class Spell_TeleTo : public SpellImpl {
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }
  bool execSpell(SpellParam& param) { return teleportTo(*param.actor, param.pos, param.target); }
  static void init(Mob& actor, CPoint targetpos, Mob* aim, SpellParam& p) { p.actor = &actor;  p.pos = targetpos; p.target = aim;  p.impl = &spell_teleTo; }
} spell_teleTo;





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
  mob.stats.healPct(-severity);
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
  case SP_TeleSelfAway:      Spell_Tele::init(actor, 44, p);      break; //return teleportSpell(actor, 44); break; // (Consider: We need a 'bullet teleport' too) This is only the 'receiver' part.
  case SP_PhaseDoor:         Spell_Tele::init(actor, 44, p);      break; //return teleportSpell(actor, 9); break;
  case SP_ConfuseSelf:    Spell_Confuse::init(actor,rnd(5,25),p); break; //return updateConfused(actor, p.confuse); break; // Careful, 'confuse' is the 'recipient part'
  case SP_Unconfuse:      Spell_Confuse::init(actor, 0, p);       break; //   return updateConfused(actor, 0); break;         // Careful, 'confuse' is the 'recipient part'
  case SP_SummonMonster:Spell_SummonMob::init(actor, p); break; //return summonSpell(actor); break;
  case SP_SummonObj:    Spell_SummonObj::init(actor, p); break; //return summonObj(actor); break;
  // it's a bullet spell, so it goes here:
  case SP_ConfuseMob:     Spell_Bullet::init(actor, item, effect, SC_Mind, p); break; // return bulletSpell(actor, item, effect, SC_Mind); break; // or gas..? // Conversely, this is the 'sender part' // It should actually just use 'confuseself' for bullet. (very fitting, how the confuse-spell has worked for the programmer himself.)
  case SP_TeleOtherAway:  Spell_Bullet::init(actor, item, SP_TeleSelfAway, SC_Mind, p); break; // return bulletSpell(actor, item, SP_TeleSelfAway, SC_Mind); break;
  case SP_TeleportTo:     Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;
  case SP_SummonHere:     Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;

  case SP_MagicMissile:   Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_FireBolt:       Spell_Bullet::init(actor, item, effect, SC_Fire, p); break; // return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_FrostBolt:      Spell_Bullet::init(actor, item, effect, SC_Frost,p); break; // return bulletSpell(actor, item, effect, SC_Frost); break;
  case SP_FireBall:       Spell_Bullet::init(actor, item, effect, SC_Fire, p); break; // return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_StinkCloud:     Spell_Bullet::init(actor, item, effect, SC_Gas, p);  break; // return bulletSpell(actor, item, effect, SC_Gas); break;
  case SP_StoneToMud:     Spell_Bullet::init(actor, item, effect, SC_Air, p);  break; // return bulletSpell(actor, item, effect, SC_Air); break;  // error/fixme: spell already knows school, so specifying it twice leads to ambiguity redundancy errors!
  case SP_WallBuilding:   Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;
  case SP_Earthquake:     Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;
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
  case SP_MagicMap: { logstr log;  log << "(magicmap not impl yet.)"; } return false;
  // case SP_Poison:       healSpell(actor); break;
  default: { logstr log; log << "err spell unknown:" << effect; } return false;
  }
  return (p.impl != NULL); // we've only prepared a spell, if we've located an 'impl'. 
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
    return true;
  }
  return false;
}

