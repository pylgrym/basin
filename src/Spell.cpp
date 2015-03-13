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


// Consider 'school' should not be extra arg to bulletSpell, because spell itself knows magicschool.
bool bulletSpell(Mob& actor, Obj* spellItem, SpellEnum effect, AttackSchool school) { // error/fixme: spell itself already knows school, so specifying it twice leads to ambiguity redundancy errors!
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

bool summonObj(Mob& actor) { // , int count) { // , CPoint pos, int radius) {
  CPoint pos = actor.pos;
  int ilevel = Levelize::suggestLevel(actor.stats.level());
  CL->map.scatterObjsAtPos(pos, rndC(1,2), ilevel, 1);
  // actor.lightArea(pos, radius);
  logstr log;
  log << "Items shimmer before you!";
  return true;
}


bool teleportTo(Mob& actor, CPoint targetpos, Mob* aim) {
  CL->map.moveMob(actor, targetpos);
  logstr log;
  log << "The air shimmers!";
  return true;
}


///////////////////////////////////////////////////////NB, 'target' here not thought through!
bool Spell::doSpell(SpellEnum effect, Mob& actor, Mob* target, std::ostream& log, Obj* item) {  

  /* FIXME: I need to clarify, the 'sender/receiver' - actor/victim - aspect of all this:
   - you can cast these on others, or on yourself.
   (the difference between you casting confusion on someone else, or on yourself
    -and them casting it on you.)
  */
  switch (effect) {
  case SP_Speedup:      return updateSpeed(actor, 2); break;
  case SP_Slowdown:     return updateSpeed(actor, 0.5); break; 
  case SP_TeleportSelfAway: return teleportSpell(actor, 44); break; // (Consider: We need a 'bullet teleport' too) This is only the 'receiver' part.
  case SP_PhaseDoor:    return teleportSpell(actor, 9); break;
  case SP_ConfuseSelf:  return updateConfused(actor, rnd(5, 25)); break; // Careful, 'confuse' is the 'recipient part'
  case SP_Unconfuse:    return updateConfused(actor, 0); break;          // Careful, 'confuse' is the 'recipient part'

  // it's a bullet spell, so it goes here:
  case SP_ConfuseMob:   return bulletSpell(actor, item, effect, SC_Mind); break; // or gas..? // Conversely, this is the 'sender part' // It should actually just use 'confuseself' for bullet. (very fitting, how the confuse-spell has worked for the programmer himself.)
  case SP_TeleportOtherAway:return bulletSpell(actor, item, SP_TeleportSelfAway, SC_Mind); break;

  case SP_TeleportTo:   return bulletSpell(actor, item, effect, SC_Magic); break;
  case SP_SummonHere:   return bulletSpell(actor, item, effect, SC_Magic); break;

  case SP_SummonMonster:return summonSpell(actor); break;
  case SP_SummonObj:    return summonObj(actor); break;

  case SP_MagicMissile: return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_FireBolt:     return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_FrostBolt:    return bulletSpell(actor, item, effect, SC_Frost); break;
  case SP_FireBall:     return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_StinkCloud:   return bulletSpell(actor, item, effect, SC_Gas); break;

  case SP_StoneToMud:   return bulletSpell(actor, item, effect, SC_Air); break;  // error/fixme: spell already knows school, so specifying it twice leads to ambiguity redundancy errors!
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
  case SP_LightDir:    return bulletSpell(actor, item, effect, SC_Light); break; // actor.pos, 3); break; // FIXME, should be zap spell instead.. (sure?)

  case SP_MagicMap: log << "(magicmap not impl yet.)"; return false;

  default: log << "err spell unknown:" << effect;  return false;
  }
  return true; 
}

// Make doors!

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
  Cuss::clear(false);
  Cuss::prtL(prompt);

  showSpellInv(); 
  return pickSpellAction();
}

void Spell::showSpellInv() { 
  /* FIXME; how about more than one page?
  */

  char ix = 'a';

  // Put column headers in front:
  std::stringstream ss1;
  ss1 << "K  L I A M Desc (key,level,ident.,learned,cost)";
    Cuss::prtL(ss1.str().c_str());  

  int count = 0;
  for (int i = SP_NoSpell+1; i < SP_MaxSpells; ++i, ++ix) {
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

    count += (d.ability);
  }
  Cuss::setTxtColor(); // Reset to default

  if (count == 0) {
    Cuss::prtL("You don't know any spells! Nothing. Nada."); 
  }

  Cuss::invalidate();
}



SpellEnum Spell::pickSpellAction() { 
  const char firstKey = 'A';
  char lastKey = 'Z'; // firstKey + objs.size() - 1; // Bag::bag.
  char lower = lastKey - ('A' - 'a');
  CString s; 
  s.Format(L"(letter [a-%c] or ESC)", lower);
  CT2A keyPrompt(s, CP_ACP);  

  int key = 0;
  for (;;) {
    key = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-item"); 
    if (key == VK_ESCAPE) {
      Cuss::clear(true);
      return SP_NoSpell; // Cancelled pick operation.
    }
    if (key >= firstKey && key <= lastKey) {
      break;
    }
    TheUI::BeepWarn();
  }

  int objIx = key - firstKey;

  int spellIx = SP_NoSpell + 1 + objIx;
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

