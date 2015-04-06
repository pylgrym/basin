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
  int ix = rnd::Rnd(SP_MaxSpells);
  return (SpellEnum)ix;
}


SpellEnum Spell::rndSpell_level(int ilevel) {
  bool demandDmg = false; // Kludge.
  for (int i = 0; i < 20; ++i) {
    int ix = rnd::Rnd(SP_MaxSpells);
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
  AND MY SPELLS ! ?
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






SpellImpl* SpellImpl::spellFromTag(const std::string& tag) {
  std::map< std::string, SpellImpl* >::iterator i;
  i = spellMap.find(tag);
  if (i != spellMap.end()) {
    return i->second;
  }
  return NULL;
}


std::set<SpellImpl*> SpellImpl::spellColl;
std::map< std::string, SpellImpl* > SpellImpl::spellMap;

void SpellImpl::initSpellMap() { // Sketch..

  std::set<SpellImpl*>::iterator i;
  for (i = spellColl.begin(); i != spellColl.end(); ++i) {
    SpellImpl* si = *i;
    spellMap[si->spelltag()] = si;
    /* 
    FIXME: use 3rd-person VERBS for spell-tags!
    
    FIXME: can't do this yet, as long as SpellImpl isn't tied to spells/types/names..SpellDesc 
    Update: spellImpl have tags now, but they are not 'unique' - 'slow' and  'speedup'
    share the same spell, but should have different args (0.5/2.0) and ids.

    Consider: raw spell functions should maybe go INTO the spellImpl classes as static function?

    FIXME/general issue:  SpellDesc struct-table approach
    and spellImpl are not tied together.
    similarly, spellTypeEnum is not tied into it.
      
    */
  }
}



bool sleepMob(Mob* target) { // (is currently accessed externally/cmds) FIXME - do we have spell for this?
  // NB, sleepMob doesn't have 'SpellImpl' yet.

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







struct Spell_X : public SpellImpl { std::string spelltag() const { return "exes"; }
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_x; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_stabs : public SpellImpl { std::string spelltag() const { return "stabs"; }
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
  // May cause bleed-DOT-effect, 50-50.
} spell_stabs; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_lunges : public SpellImpl { std::string spelltag() const { return "lunges"; }
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
  // What does lunge do?
} spell_lunges; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_charges : public SpellImpl {  std::string spelltag() const { return "charges"; }
  // Charge does the same as rush.. mob moves up to target, and possibly short-duration-stuns/confuses him (1 turn?)
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_charges; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_bumps : public SpellImpl {  std::string spelltag() const { return "bumps"; }
  // Bump will bump you 1 field backwards; further: If it bumps you up against anything, it hits harder,
  // so you should 'keep your back free' when you fight a bumping mob.
  // Alternatively, fighting with your back against a wall, might protect you from 'kick-back stun effects'?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_bumps; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

// Spell 'embeds' already exists.
/* Idea: elemental-shield, e.g. fire-shield, that causes some kind of effect to 
mobs that brush up against you, and/or protects you (e.g. against frost attacks/mobs.)
*/

struct Spell_FireShield : public SpellImpl {  std::string spelltag() const { return "burns"; }
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_burns; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_Holds : public SpellImpl {  std::string spelltag() const { return "holds"; }
  // aliases: grab/lock/pin
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_holds; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_Swipes : public SpellImpl {  std::string spelltag() const { return "swipes"; }
  /*'dragon tail swipe': It will knock you 'sideways' 1 square, hurt you, and probably unbalance you.
   I don't know, if it will interact with walls?
  Alias: lashes (tail lashes) - possible 'pain dot' (from lashing..)
  */
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_swipes; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_Summons : public SpellImpl {  std::string spelltag() const { return "summons"; }
  // already exists: summons helper monsters. Needs own phrasing (so it doesn't say 'summons you'.)
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_summons; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_Spits : public SpellImpl {  std::string spelltag() const { return "spits"; }
  // Acid/fire/burn effect - may apply fire/burn/corrode dot? possibly blinding/poision
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_spits; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_Hisses : public SpellImpl {  std::string spelltag() const { return "hisses"; }
  // hiss may frighten you (alias for fear), or alert nearby monsters.
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_hisses; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_Bites : public SpellImpl {  std::string spelltag() const { return "bites"; }
 /* May infect with disease, rot, sickness.
 */
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_bites; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_slimes : public SpellImpl {  std::string spelltag() const { return "slimes"; }
  /* Slime will DOT-slow your speed, possibly corrode or poison.
  might also leave 'bad on ground'.
  */
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_slimes; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_corrodes : public SpellImpl {  std::string spelltag() const { return "corrodes"; }
/* corrode would damage certain kinds of things in your inventory,
possibly with (beneficial?) side effects (e.g. prematurely triggering your potion effects ?),
  might it also make you drop things?
*/
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_corrodes; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_splits : public SpellImpl {  std::string spelltag() const { return "splits"; }
  /* mob splits in two, "more-than-halving" HP, ie 200 HP -> 120+120 HP.
  action for oozes, worms, rats, vermin, gnats, lice, fleas.
  */
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_splits; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_gazes : public SpellImpl {  std::string spelltag() const { return "gazes"; }
/* aliases: stares, x.
effect: might paralyse you, confuse you, stone you, petrify-you, blind.. you, slow you? charm you?
*/
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_gazes; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


/* idea: immaterial 'mist'like monsters, that might move through you or 'possess' you? 'inhale' effect?
*/

/* idea: avoiding breathing in temp gas effects dotting floor? green poison cloud?
*/

struct Spell_swirls : public SpellImpl {  std::string spelltag() const { return "swirls"; }
/* ghost-ethereal attack */
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_swirls; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_bores : public SpellImpl {  std::string spelltag() const { return "bores"; }
/* bore might do bleed/poison/pain effect?
*/
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_bores; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

// stagger?

struct Spell_claws : public SpellImpl { std::string spelltag() const { return "claws"; }
/* claw may infect/bleed effect?
*/
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_claws; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

// molten/melt
struct Spell_glows : public SpellImpl {  std::string spelltag() const { return "burns"; }  
  /* alias: melt.
  applies a fire DOT effect 
  */
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_glows; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_freezes : public SpellImpl {  std::string spelltag() const { return "freezes"; }
/* will root you in place and slow you:
 - you can't MOVE until you 'melt' somehow, and
 you can only hit twice as slow, because you are frozen-slowed.
 think of ideas to counter/lessen this.
*/
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_freezes; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

// Grab and hold are rooting effects, or possibly they mean the mob DRAGS you along!

struct Spell_murmurs : public SpellImpl {
  /* alias: whisper?
  murmurs are temp-stat-reducing effects, that ghosts cause.
  */
  std::string spelltag() const { return "murmurs"; }
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_murmurs; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_Paralyze : public SpellImpl {
  /* paralyze is a .. simple? impassive-for-3-turns (?) effect.. 
  any way to spice it up / make it interesting instead of just immensely annoying?
  possibly a timeout, that means you are immmune to it for 3 turns or 1 turn or something, when it's happened?
  'diminishing returns'.
  */
  std::string spelltag() const { return "paralyzes"; }  
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_paralyzes; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_charms : public SpellImpl {
  /* you hand over one of your items or some of your gold? or what?
  possibly works as a buff on that mob itself, to ensure you can't attack it/hurt it.. temp?
  */
  std::string spelltag() const { return "charms"; }
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_charms; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

// drool.

// slimed squares will slow you or dot you?

struct Spell_sleeps : public SpellImpl {
  /* how to interact with combat? that mob gets away? */
  std::string spelltag() const { return "sleeps"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_sleeps; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_blinds : public SpellImpl {
  /*blind: room turns dark, so mainly a 'rendering' thing - might also affect spells/reading..?*/
  std::string spelltag() const { return "blinds"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_blinds; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

/* ideas: 40-30-20-10 dots, and 15-15-15-15 dots, and  10-20-30-40 dots..
the 'riser' dots, should be cancelled somehow, tactics-wise..
*/

struct Spell_strangles : public SpellImpl {
  std::string spelltag() const { return "strangles"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_strangles; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_dooms : public SpellImpl {
  /* a count-down, where you must get out of LOS bbefore trigger/launch.
  */
  std::string spelltag() const { return "dooms"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_dooms; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_screams : public SpellImpl {
  /// aliases: frightens, scares?
  // A confusion-attack/stun/fear attack?
  std::string spelltag() const { return "screams"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_screams; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_haunts : public SpellImpl {
  // temp-stat-lower?
  std::string spelltag() const { return "haunts"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_haunts; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 


struct Spell_wails : public SpellImpl {
  std::string spelltag() const { return "wails"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_wails; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 

struct Spell_scratches : public SpellImpl {
  // infects/dot:
  std::string spelltag() const { return "scratches"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_scratches; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 
/*
struct Spell_xs : public SpellImpl {
  std::string spelltag() const { return "xs"; } // lulls you to sleep/dozes you off to sleep?
  bool Do(SParam& p) { p.actor->stats.s_afraid.updateEffect(p.tmpEffect); return true; } // execSpell
} spell_xs; //SpellParam / logstr log; log << p.actor.pronoun() << " exes " << p.target->pronoun() << "!"; 
*/







class Spell_Speed : public SpellImpl { 
public:
  std::string spelltag() const { return "speeds"; }

  static bool exec2(Mob& actor, double factor) { // updateSpeed
    playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.

    logstr log;
    // 0.5 is fast, 2.0 is slow.
    if (factor < 1.0) { log << actor.the_mob() << " speed" << actor.verbS() << " up."; }
    else { log << actor.the_mob() << " slow" << actor.verbS() << " down."; }

    /* I actually experienced a dragon that just
    kept speeding up by factor of x2 continously - I never got my turn back..
    */

    if (factor < 1.0 && actor.stats.mob_speed > 0.25) {
      actor.stats.mob_speed *= factor; // max speed is 0.25..
    }
    if (factor > 1.0 && actor.stats.mob_speed < 2) {
      actor.stats.mob_speed *= factor; // slowest speed is 2.0..
    }
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.factor); } // updateSpeed
  static void init(Mob& actor, double factor, SpellParam& p) { p.actor = &actor;  p.factor = factor; p.impl = &spell_speed; }
} spell_speed;








class Spell_Tele : public SpellImpl { 
public:
  std::string spelltag() const { return "teleports"; }

  static bool exec2(Mob& actor, int range) { // teleportSpell
    playSound(L"sounds\\sfxr\\teleport.wav"); // teleport-spell cast.

    {
      logstr log;
      if (actor.isPlayer()) {
        log << "Your body shifts in time and space.";
      }
      else {
        log << actor.the_mob() << " shifts in time and space.";
      }
    }

    for (int i = 0; i < 15; ++i) { // We try a number of times, to avoid teleporting into rock.
      CPoint delta;
      for (;;) {
        delta.x = rnd::rndC(-range, range); delta.y = rnd::rndC(-range, range);
        CPoint newPos = actor.pos + delta;
        if (CL->map.legalPos(newPos) && !CL->map[newPos].blocked()) { break; }
      }
      WalkCmd cmd(actor, delta.x, delta.y, true);
      // Possibly check 'legal' (for mapPosLegal), even before calling Do.
      logstr log;
      if (cmd.Do(log)) { return true; } // Otherwise, keep trying different directions.
    }
    return false; // It never worked. Not normal.
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.range); } // teleportSpell
  static void init(Mob& actor, int range, SpellParam& p) { p.actor = &actor;  p.range = range; p.impl = &spell_tele; }
} spell_tele;




/* Idea: 2-stage attack, where a mob will prepare an announced strike,
that suggests you move away from him on next turn 
(e.g. if you remain next to him, he'll hit you HARD (not insta-kill),
so he sorts of forces your hand about 'better step away in some direction,
or suffer the consequences'.)
*/

/* DONE:fixme, entire map should be drawn/offset 2 first rows, so we can see them.
something to do with viewport.
*/




class Spell_Confuse : public SpellImpl { public:
  std::string spelltag() const { return "confuses"; }

  static bool exec2(Mob& actor, int confuseCount) { // updateConfused
    if (confuseCount > 0) { // If counter is set positive, a confusion-spell has been cast on someone.
      playSound(L"sounds\\sfxr\\confuse.wav"); // confusion-spell cast.
    }

    logstr log;
    if (confuseCount > 0) { log << actor.pronoun() << " feel" << actor.verbS() << " confused."; }
    else { log << actor.pronoun() << " feel" << actor.verbS() << " less confused."; }
    actor.stats.s_confused.updateEffect(confuseCount);
    return true;
  }


  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.tmpEffect); } // updateConfused confuse); }
  static void init(Mob& actor, int confuse, SpellParam& p) { p.actor = &actor;  p.tmpEffect = confuse; p.impl = &spell_confuse; }
} spell_confuse;




class Spell_Fear: public SpellImpl { public:
  std::string spelltag() const { return "fears"; }


  static bool exec2(Mob& actor, int count) { // updateFear
    logstr log;
    if (count > 0) { log << actor.pronoun() << " is afraid!"; }
    else { log << actor.pronoun() << " appear" << actor.verbS() << " less afraid."; }
    actor.stats.s_afraid.updateEffect(count); return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.tmpEffect); } // updateFear
  static void init(Mob& actor, int dur, SpellParam& p) { p.actor = &actor;  p.tmpEffect = dur; p.impl = &spell_fear; }
} spell_fear;



class Spell_Blind: public SpellImpl { public:
  std::string spelltag() const { return "blinds"; }
  static bool exec2(Mob& actor, int count) { // updateBlind
    logstr log;
    if (count > 0) { log << actor.pronoun() << " can't see!"; }
    else { log << actor.pronoun() << " can see again."; }
    actor.stats.s_blinded.updateEffect(count); return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.tmpEffect); } // updateBlind
  static void init(Mob& actor, int dur, SpellParam& p) { p.actor = &actor;  p.tmpEffect = dur; p.impl = &spell_blind; }
} spell_blind;



class Spell_Root : public SpellImpl { public:
  std::string spelltag() const { return "roots"; }
  static bool exec2(Mob& actor, int count) { // updateRooted
    logstr log;
    if (count > 0) { log << actor.pronoun() << " appear" << actor.verbS() << " rooted in place!"; }
    else { log << actor.pronoun() << " can move freely again."; }
    actor.stats.s_rooted.updateEffect(count); return true;
  }


  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.tmpEffect); }// updateRooted
  static void init(Mob& actor, int dur, SpellParam& p) { p.actor = &actor;  p.tmpEffect = dur; p.impl = &spell_root; }
} spell_root;



class Spell_Poison : public SpellImpl { public:
  std::string spelltag() const { return "poisons"; }

  static bool exec2(Mob& actor, int count) { // updatePoisoned
    logstr log;
    if (count > 0) { log << actor.pronoun() << " appear" << actor.verbS() << " poisoned!"; }
    else { log << actor.pronoun() << " appear" << actor.verbS() << " unpoisoned."; }
    actor.stats.s_poisoned.updateEffect(count); return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.tmpEffect); } // updatePoisoned
  static void init(Mob& actor, int dur, SpellParam& p) { p.actor = &actor;  p.tmpEffect = dur; p.impl = &spell_poison; }
} spell_poison;





class Spell_Eat: public SpellImpl { 
public:
  std::string spelltag() const { return "eats"; }

  static bool exec2(Mob& actor, int deltaFood) { // eatSpell
    {
      logstr log; log << actor.the_mob() << " eat" << actor.verbS() << " a bit and feel" << actor.verbS() << " less hungry!";
      actor.stats.hunger += deltaFood; // Eat 300 something.
    }
    actor.stats.healPct(25, &actor);
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.deltaFood); } // eatSpell
  static void init(Mob& actor, int deltaFood, SpellParam& p) { p.actor = &actor;  p.deltaFood = deltaFood; p.impl = &spell_eat; }
} spell_eat;



bool eatSpell(Mob& actor, int deltaFood) { // Old external/direct accessors.
  return Spell_Eat::exec2(actor, deltaFood); // hack-kludge.
}



class Spell_HealPct: public SpellImpl { 
public:
  std::string spelltag() const { return "heals"; }

  static bool exec2(Mob& actor, int percent) { // healSpellPct
    {
      logstr log;
      if (percent > 0) {
        log << actor.pronoun() << " feel" << actor.verbS() << " healing energies.";
      }
      if (percent < 0) {
        log << actor.pronoun() << " feel" << actor.verbS() << " sick.";
      }
    }
    actor.stats.healPct(percent, &actor);
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.healPct); } // healSpellPct
  static void init(Mob& actor, int healPct, SpellParam& p) { p.actor = &actor;  p.healPct = healPct; p.impl = &spell_healPct; }
} spell_healPct;



class Spell_HealDice: public SpellImpl { 
public:
  std::string spelltag() const { return "healdices"; }

  static bool exec2(Mob& actor, Dice dice) { // healSpellDice
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

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.healDice); } // healSpellDice
  static void init(Mob& actor, Dice healDice, SpellParam& p) { p.actor = &actor;  p.healDice = healDice; p.impl = &spell_healDice; }
} spell_healDice;




class Spell_ManaPct: public SpellImpl { 
public:
  std::string spelltag() const { return "manapct"; } // energizes

  static bool exec2(Mob& actor, int percent) { // manaSpellPct
    {
      logstr log;
      if (percent > 0) {
        log << actor.pronoun() << " feel" << actor.verbS() << " mana flowing through 'you'.";
      }
      if (percent < 0) {
        log << actor.pronoun() << " feel" << actor.verbS() << " mana draining away from 'you'.";
      }
    }
    actor.stats.manaPct(percent, &actor);
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.manaPct); } // manaSpellPct
  static void init(Mob& actor, int manaPct, SpellParam& p) { p.actor = &actor;  p.manaPct = manaPct; p.impl = &spell_manaPct; }
} spell_manaPct;




class Spell_Bullet: public SpellImpl { 
public:
  std::string spelltag() const { return "zaps"; } //  bullets"; }

  bool getParams(SpellParam& param) {
    return getParamsDIR(param);
  }

  // Consider: 'school' should not be extra arg to bulletSpell, because spell itself knows magicschool.
  static bool exec2(Mob& actor, Obj* spellItem, SpellEnum effect, AttackSchool school, CPoint dir) { // bulletSpell error/fixme: spell itself already knows school, so specifying it twice leads to ambiguity redundancy errors!
    ZapCmd cmd(spellItem, actor, effect, school);
    cmd.mobZapDir = dir;
    logstr log;
    return cmd.Do(log);
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

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.item, param.effect, param.school, param.dir);  } // bulletSpell
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
  std::string what() {return "treasure";}
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



class Spell_Detect : public SpellImpl {
public:
  std::string spelltag() const { return "detects"; }

  static bool exec2(Mob& actor, SpellEnum effect) { // spellDetect
    CPoint pos = actor.pos;
    int rad = 23;
    switch (effect) {
    case SP_DetectDoor:    return detectDoors(pos, rad, actor);
    case SP_DetectTrap:    return detectTraps(pos, rad, actor);
    case SP_DetectTreasure:return detectTreasure(pos, rad, actor);
    case SP_DetectObject:  return detectObj(pos, rad, actor);
    case SP_DetectMobs:    return detectMobs(pos, rad, actor);
    }
    assert(false);  // spellDetect shouldn't happen.
    return false;
  }


  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.effect); } // spellDetect Consider: we could impl directly here!

  static void init(Mob& actor, SpellEnum effect, SpellParam& p) { p.actor = &actor; p.effect = effect; p.impl = &spell_detect; }
} spell_detect;



class Spell_Light: public SpellImpl { 
public:
  std::string spelltag() const { return "lights"; }
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }

  static bool exec2(Mob& actor, CPoint pos, int radius) { // lightSpell
    /* idea: that light-area sets s light-strength on the tiles.
    */
    // Do a 'light' command:
    actor.lightArea(pos, radius);
    logstr log;
    log << "Light floods around you.";
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.pos, param.radius);  }// lightSpell
  static void init(Mob& actor, CPoint pos, int radius, SpellParam& p) { p.actor = &actor;  p.pos = pos; p.radius = radius;  p.impl = &spell_light; }
} spell_light;



class Spell_SummonMob : public SpellImpl {
public:
  std::string spelltag() const { return "summons"; }
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }

  static bool exec2(Mob& actor) { //summonSpell , CPoint pos, int radius) {
    // CONSIDER: Summon-Mob-Type, e.g. undead, demon, dragon, elemental, etc.
    CPoint pos = actor.pos;
    int mlevel = rnd::rndC(1, 2) + Levelize::suggestLevel(actor.stats.level()); // Scary - a bit higher than we'd like :-)
    CL->map.scatterMobsAtPos(pos, 1, mlevel, 1);
    // actor.lightArea(pos, radius);
    logstr log;
    log << "A monster shimmers before you!";
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor); } /// summonSpell
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_summonMob; }
} spell_summonMob;


class Spell_SummonObj : public SpellImpl {
public:
  std::string spelltag() const { return "summonsobj"; }
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }

  static bool exec2(Mob& actor) { //summonObj , int count) { // , CPoint pos, int radius) {
    CPoint pos = actor.pos;
    int ilevel = Levelize::suggestLevel(actor.stats.level());
    CL->map.scatterObjsAtPos(pos, rnd::rndC(1, 2), ilevel, 1);
    // actor.lightArea(pos, radius);
    logstr log;
    log << "Items shimmer before you!";
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor); } // summonObj
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_summonObj; }
} spell_summonObj;





bool teleportSwap(Mob& actor, Mob& target, bool announce) {  // used by Cmds.
  // it's tricky, because we want each other's space..
  CPoint actorNewpos = target.pos; 
  CPoint targetNewpos = actor.pos;

  // This special mob-mover doesn't clear the previous space, so the two mobs don't sabotage each other..
  CL->map.setMobForce(actor, actorNewpos, true);
  CL->map.setMobForce(target, targetNewpos, true);
  if (actor.isPlayer())  {  actor.lightWalls(); }  // Fixme - moving always needs this? (we don't want move+light everytime.)
  if (target.isPlayer()) { target.lightWalls(); }  // Fixme - moving always needs this? (we don't want move+light everytime.)

  if (announce) {
    logstr log;
    log << "The air shimmers around both of you!";
  }
  return true;
}



class Spell_TeleTo : public SpellImpl { public:
  std::string spelltag() const { return "teleportsto"; }
  // bool getParams(SpellParam& param) { param.dir = CPoint(1, 0);  return true; }

  static bool exec2(Mob& actor, CPoint targetpos, bool announce) { // teleportTo
    CL->map.moveMob(actor, targetpos);
    if (actor.isPlayer()) { actor.lightWalls(); }  // Fixme - moving always needs this? (we don't want move+light everytime.)

    if (announce) {
      logstr log;
      log << "The air shimmers!";
    }
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.pos, true); } // teleportTo
  static void init(Mob& actor, CPoint targetpos, SpellParam& p) { p.actor = &actor;  p.pos = targetpos; p.impl = &spell_teleTo; }
} spell_teleTo;


bool extTeleportTo(Mob& actor, CPoint targetpos, bool announce) { // Old external/direct accessors.
  return Spell_TeleTo::exec2(actor, targetpos, announce);
}

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




/* The basic idea with a 3rd-person-verb as spell-tag is sympathetic,
but it can ony be a default - the spell class itself must
have an overridable text-describer, that defaults to the raw verb,
but that can  be overridden to 'teleports to you, heals you,restores your mana' or whatever
describes more complex actions.
*/

class Spell_Rush: public SpellImpl { public:
  std::string spelltag() const { return "rushes"; }
  bool getParams(SpellParam& param) {
    return Spell_Bullet::getParamsDIR(param); 
  }

  static bool exec2(Mob& actor, CPoint dir, std::string verb) { //spellRush
    // NB! if you do this without hitting a mob, you'll hurt yourself badly for 2/3 of your (remaining) health!
    // (I want to discourage using it for plain moving/fast moving..)
    // Actually, it could still be used to flee fast along a corridor, to get quickly away from a mob (at the price of 2/3 hp..)

    playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
    // { logstr log; log << "You would rush headfirst into the mob..";  }
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

      Mob* target = cell->creature.m;
      {
        logstr log;
        if (!verb.empty()) {
          log << verb;
        }
        else {
          if (actor.isPlayer()) {
            log << "You rush into " << target->the_mob() << "!";
          }
          else {
            log << actor.the_mob() << " rushes into " << target->the_mob() << "!";  // "You rush into " 
          }
        }
      }

      const bool doOverrideHit = true;
      HitCmd rush(NULL, actor, dir.x, dir.y, SC_Phys, SP_Rush, doOverrideHit); // FIXME; how much dmg does it do, and does it stun/confuse him?
      logstr log;
      return rush.Do(log);
    }

    if (cell->envir.blocked()) {
      if (actor.isPlayer()) { logstr log; log << "Augh! You rush into the wall, seriously hurting yourself!"; }
      else { logstr log; log << "Ouch! " << actor.the_mob() << " rushes into the wall, seriously hurting itself!"; }

      Spell_HealPct::exec2(actor, -66); // Loose 66 pct of health! //  healSpellPct
      return true;
    }
    logstr log; log << "Weird, your(?) rush leads nowhere?";
    return true;
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.dir, "");  } // spellRush Consider: we could impl directly here!
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_rush; }
} spell_rush;





class Spell_Crush: public SpellImpl { public:
  std::string spelltag() const { return "crushes"; }
  bool getParams(SpellParam& p) { return getParamsCRUSH(p); }

  static bool getParamsCRUSH(SpellParam& p) { 
    bool paramOK = Spell_Bullet::getParamsDIR(p); 
    if (!paramOK) { return false;  }
    p.pos = p.actor->pos + p.dir;
    p.target = CL->map[p.pos].creature.m;
    if (p.target == NULL) { if (p.actor->isPlayer()) { logstr log; log << "But there is noone there to crush?"; } return false; }
    if (!CL->map[p.pos+p.dir].envir.blocked()) { if (p.actor->isPlayer()) { logstr log; log << "But there is no wall to crush against?"; } return false; }
    return true;
  }

  bool exec2(Mob& actor, Mob& target, CPoint dir) { // spellCrush
    playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
    { logstr log; log << actor.the_mob() << " would pin the mob against the wall.."; }
    // NB! Must check mob is adj, and wall on other side.. Make a toolbox to build spells..
    // Consider: maybe it's HitCmd instead?
    // Todo: target ought to be passed along to hit/zap cmd..
    ZapCmd cmd(NULL, actor, SP_Crush, SC_Phys); // school);
    cmd.mobZapDir = dir;
    logstr log;
    return cmd.Do(log);
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, *param.target, param.dir);  } // Consider: we could impl directly here! // spellCrush
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_crush; }
} spell_crush;





class Spell_Embed: public SpellImpl { public:
  std::string spelltag() const { return "embeds"; }
  bool getParams(SpellParam& param) { // Embed has same requirements as Crush (wall on other side, and mob in between, directly next to you.)
    return Spell_Crush::getParamsCRUSH(param); 
  }

  bool exec2(Mob& actor, Mob& target, CPoint dir) { // spellEmbed
    playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
    { logstr log; log << actor.the_mob() << " attempt" << actor.verbS() << " to embed " << target.the_mob() << " in the rock.."; }
    // Todo: must check mob is adj, and wall on other side.. Make a toolbox to build spells..

    // (50%done)-FIXME - this does not push mob into wall, and move you..
    CPoint embedPos = (target.pos + dir);
    target.moveM(embedPos); //CL->map.moveMob(target, target.pos);

    ZapCmd cmd(NULL, actor, SP_Embed, SC_Phys); // school);
    cmd.mobZapDir = dir;
    logstr log;
    return cmd.Do(log);
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, *param.target, param.dir); } // Consider: we could impl directly here! // spellEmbed
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_embed; }
} spell_embed;




/* thoughts: i should consider making 'floodfill-laby' prettier.
DONE: i should make restore mana
*/

class Spell_Shove: public SpellImpl { public:
  std::string spelltag() const { return "shoves"; }
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

  static bool exec2(Mob& actor, Mob& target, CPoint dir) { // spellShove
    playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
    // { logstr log; log << "You would shove the mob along the ground.."; }

    //DONE:Todo: must check mob is adj, and wall on other side.. Make a toolbox to build spells..
    // was: - this does not push mob into wall, and move you..
    // (class Spell_Shove handles the checks for us.)

    std::stringstream ss;
    ss << actor.pronoun() << " shove" << actor.verbS() << " " << target.the_mob() << "!";
    std::string s = ss.str();

    return Spell_Rush::exec2(target, dir, s); // spellRush hack: move mob instead.. // fixme.. hitting the MOB for 66% of health is too extreme.
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, *param.target, param.dir);  } // Consider: we could impl directly here! // spellShove(
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_shove; }
} spell_shove;




// DONE: you use the thingey -> real name!
// DONE: that way is blocked by x.

/* consider, 'you use the x', should be short-circuited for staffs/wands
(go directly to 'direction?')
*/

class Spell_Tackle: public SpellImpl { public:
  std::string spelltag() const { return "tackle"; }

  bool getParams(SpellParam& param) { return getParamsTACKLE(param); }

  static bool getParamsTACKLE(SpellParam& p) { 
    bool paramOK = Spell_Bullet::getParamsDIR(p); 
    if (!paramOK) { return false;  }
    p.pos = p.actor->pos + p.dir;

    // tackle doesn't work this way, this is left-over code from crush:
    // p.target = CL->map[p.pos].creature.m;
    // FIXME - tackle doesn't work this way! it's more akin to rush!
    // if (p.target == NULL) { if (p.actor->isPlayer()) { logstr log; log << "But there is noone there to tackle?"; } return false; }
    // if (!CL->map[p.pos+p.dir].envir.blocked()) { if (p.actor->isPlayer()) { logstr log; log << "But there is no wall to crush against?"; } return false; }

    return true;
  }

  static bool exec2(Mob& actor, Mob* target, CPoint dir) { // spellTackle
    // JG -something is wrong, 'target' is a nullptr/not known!
    // fixme -tackle seems to require being next to mob, that doesn't sound right?
    /* fixme - the physical spells don't seem to either identify or eat-charges correctly - 'shove' never eats its charges?
    */
    // fiXmE - is 'target anything yet? (it might be,  because we assume both are next to each other? but actually, 'tackle' may use run-lead-up?
    playSound(L"sounds\\sfxr\\negative.wav"); // speed/slow spell.
    // { logstr log; log << "You would tackle the mob.."; }

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
      if (actor.isPlayer()) { logstr log; log << "Augh! You tackle into the wall, seriously hurting yourself!"; }
      else { logstr log; log << "Ouch! " << actor.the_mob() << " tackles into the wall, seriously hurting itself!"; }
      Spell_HealPct::exec2(actor, -66); // Loose 66 pct of health! //  healSpellPct
      return true;
    }

    if (cell->creature.empty()) { // bad unexpected we didn't bump into an enemy?
      { logstr log; log << "Why is there noone here?"; }
      return true;
    }
    Mob* mob = cell->creature.m;

    // Second stage, now both mob and actor must slide along.
    { logstr log; log << actor.pronoun() << " tackle" << actor.verbS() << " " << mob->the_mob() << "!"; }

    CPoint newMobPos = newActorPos + dir;
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
    HitCmd tackle(NULL, actor, dir.x, dir.y, SC_Phys, SP_Tackle, overrideHit); // FIXME; how much dmg does it do, and does it stun/confuse him?
    logstr log;
    return tackle.Do(log);
  }

  logstr log; log << "Weird, your(?) tackle leads nowhere?";

  /*
  ZapCmd cmd(NULL, actor, SP_Tackle, SC_Phys); // school);
  cmd.mobZapDir = dir;
  logstr log;
  return cmd.Do(log);
  */
  }

  bool execSpell(SpellParam& param) { return exec2(*param.actor, param.target, param.dir);  } // Consider: we could impl directly here! // spellTackle
  static void init(Mob& actor, SpellParam& p) { p.actor = &actor;  p.impl = &spell_tackle; }
} spell_tackle;


// zap which?
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
    if (YN_Key == 'Y') { bFound = true;  break; }
  } // Loop until Y/N/Esc key.

  bool bFail = rnd::oneIn(3); 
  if (!bFail) {
    logstr log; log << "You pull it off! ..";
    return true;
  }

  err << "You fumble and damage your health.";
  int severity = rnd::Rnd(25, 50);
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


bool Spell::prepareSpell2(SpellParam& p, SpellImpl* pImpl, class Mob& actor, Mob* target, class Obj* item) {
  p.actor = &actor;
  p.target = target;
  p.item = item;

  p.dir = actor.playerDir();
  p.pos = actor.pos;
  p.impl = pImpl;

  /*
  p.range
  p.radius
  p.manaPct
  p.healPct
  p.factor
  p.effect
  p.deltaFood
  */
 
  return true;
}

/* THOUGHT : bulletspell is a 'bad hack' that i should untangle,
so I can get my spells straight.
*/

///////////////////////////////////////////////////////NB, 'target' here not thought through!
bool Spell::prepareSpell(SpellParam& p, SpellEnum effect, Mob& actor, Mob* target, Obj* item) {  
  // prepareSpell fills out a SpellParam! - makes sure e.g. bullet-aim-dir is known
  switch (effect) {
  case SP_Speedup:          Spell_Speed::init(actor, 0.5, p);     break; // SPEED-FACTOR. return updateSpeed(actor, 2); break;
  case SP_Slowdown:         Spell_Speed::init(actor, 2.0, p);     break; // SPEED-FACTOR. return updateSpeed(actor, 0.5); break; 

  case SP_ConfuseSelf:    Spell_Confuse::init(actor,rnd::Rnd(5,25),p); break; // DURATION. return updateConfused(actor, p.confuse); break; // Careful, 'confuse' is the 'recipient part'
  case SP_Unconfuse:      Spell_Confuse::init(actor, 0, p);       break; //   NO ARGS return updateConfused(actor, 0); break;         // Careful, 'confuse' is the 'recipient part'
  case SP_ConfuseMob:      Spell_Bullet::init(actor, item, effect, SC_Mind, p); break; // return bulletSpell(actor, item, effect, SC_Mind); break; // or gas..? // Conversely, this is the 'sender part' // It should actually just use 'confuseself' for bullet. (very fitting, how the confuse-spell has worked for the programmer himself.)

    // sleep other, here?
  case SP_SleepOther:      Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_TeleSelfAway:      Spell_Tele::init(actor, 44, p);      break; // TELEPORT_RANGE. return teleportSpell(actor, 44); break; // (Consider: We need a 'bullet teleport' too) This is only the 'receiver' part.
  case SP_TeleOtherAway:   Spell_Bullet::init(actor, item, SP_TeleSelfAway, SC_Mind, p); break; // return bulletSpell(actor, item, SP_TeleSelfAway, SC_Mind); break;


  case SP_SummonHere:      Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // x goes to me.
  case SP_SummonMonster:Spell_SummonMob::init(actor, p); break; // NO ARGS return summonSpell(actor); break;
  case SP_SummonObj:    Spell_SummonObj::init(actor, p); break; // NO ARGS return summonObj(actor); break;
  // it's a bullet spell, so it goes here:
  case SP_TeleportTo:     Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // i go to mob x.
  case SP_TeleSwap:       Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // i swap with x. // hmm, doesn't seem to work?

  case SP_MagicMissile:   Spell_Bullet::init(actor, item, effect, SC_Magic,p); break; // return bulletSpell(actor, item, effect, SC_Magic); break;  
  case SP_FireBolt:       Spell_Bullet::init(actor, item, effect, SC_Fire, p); break; // return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_FrostBolt:      Spell_Bullet::init(actor, item, effect, SC_Frost,p); break; // return bulletSpell(actor, item, effect, SC_Frost); break;
  case SP_FireBall:       Spell_Bullet::init(actor, item, effect, SC_Fire, p); break; // return bulletSpell(actor, item, effect, SC_Fire); break;
  case SP_StinkCloud:     Spell_Bullet::init(actor, item, effect, SC_Gas, p);  break; // return bulletSpell(actor, item, effect, SC_Gas); break;
  case SP_StoneToMud:     Spell_Bullet::init(actor, item, effect, SC_Air, p);  break; // return bulletSpell(actor, item, effect, SC_Air); break;  // error/fixme: spell already knows school, so specifying it twice leads to ambiguity redundancy errors!
  case SP_WallBuilding:   Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;
  case SP_Earthquake:     Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;
  case SP_FocusBlast:     Spell_Bullet::init(actor, item, effect, SC_Earth, p);break; // return bulletSpell(actor, item, effect, SC_Earth); break;

  case SP_Rush:             Spell_Rush::init(actor, p);                        break; // NO ARGS 
  case SP_Crush:           Spell_Crush::init(actor, p);                        break; // NO ARGS
  case SP_Embed:           Spell_Embed::init(actor, p);                        break; // NO ARGS
  case SP_Shove:           Spell_Shove::init(actor, p);                        break; // NO ARGS
  case SP_Tackle:         Spell_Tackle::init(actor, p);                        break; // NO ARGS

                       // FiXME, dragon-breath is NOT tackle!
  case SP_Breath:         Spell_Tackle::init(actor, p);                        break; // NO ARGS

  case SP_Eat:         Spell_Eat::init(actor, item ? item->itemUnits : 250, p);break; // CHARGES/ITEMS/UNITS return eatSpell(actor,p.deltaFood); break;
  case SP_Sick:        Spell_HealPct::init(actor, -35, p);                     break; // UNITS return healSpellPct(actor,p.healPct); break;
  // Heal dice design: lots of small dice, so you always get some healing.
  case SP_Heal_light:   Spell_HealDice::init(actor, Dice(4,3), p);             break; //return healSpellDice(actor, p.healDice); break;
  case SP_Heal_minor:   Spell_HealDice::init(actor, Dice(6,3), p);             break; //return healSpellDice(actor, Dice(6,  3)); break;
  case SP_Heal_mod:     Spell_HealDice::init(actor, Dice(8,3), p);             break; //return healSpellDice(actor, Dice(8,  3)); break;
  case SP_Heal_serious: Spell_HealDice::init(actor, Dice(10,3), p);            break; //return healSpellDice(actor, Dice(10, 3)); break;
  case SP_Heal_crit:    Spell_HealDice::init(actor, Dice(12,3), p);            break; //return healSpellDice(actor, Dice(12, 3)); break;
  case SP_LightArea:       Spell_Light::init(actor, actor.pos,4, p);           break; // RADIUS return lightSpell(actor, actor.pos,4); break;
  case SP_LightDir:       Spell_Bullet::init(actor, item, effect, SC_Light, p);break; //return bulletSpell(actor, item, effect, SC_Light); break; // actor.pos, 3); break; // FIXME, should be zap spell instead.. (sure?)

  case SP_DetectDoor: case SP_DetectTrap: case SP_DetectTreasure: case SP_DetectObject: case SP_DetectMobs:
                          Spell_Detect::init(actor, effect,p);                 break;

  case SP_MagicMap:        Spell_Light::init(actor, actor.pos,22, p);          break; //RANGE/RADIUS - return lightSpell(actor, actor.pos,4); break;
  // case SP_MagicMap: { logstr log;  log << "(magicmap not impl yet.)"; } return false;
  case SP_PhaseDoor:        Spell_Tele::init(actor, 44, p);      break; //44? REALLY? range/radius - return teleportSpell(actor, 9); break;

    // Idea -it could be partial with blanks/particles?
    /* todo:
void detectObj(CPoint pos, int radius) { return detectCells(pos, radius, IsObj()); } //, CheckCellBase& checker) {
void detectMobs(CPoint pos, int radius) { return detectCells(pos, radius, IsMob()); } //, CheckCellBase& checker) {
void detectDoors(CPoint pos, int radius) { return detectCells(pos, radius, IsDoor()); } //, CheckCellBase& checker) {
void detectTraps(CPoint pos, int radius) { return detectCells(pos, radius, IsTrap()); } //, CheckCellBase& checker) {
void detectTreasure(CPoint pos, int radius) { return detectCells(pos, radius, IsTreasure()); } //, CheckCellBase& checker) {
    */

  case SP_RestoreMana:   Spell_ManaPct::init(actor, 33, p);                    break; // UNITS return healSpellPct(actor,p.healPct); break;
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

/* it should be possible to simplify this greatly.
spells will have tag-names, and go in a string-map.
  input parameters should be passed in a general 'universal' inputparam-struct,
instead of all those init-calls.
  I should consider if I can throw away the 'getparams' pre-part.

I should consider if spellDesc part could be integrated in the spell classes.
(ie add things like min/max range to the class instead?)
Advantage of class-approach, is that I can extend/add aspects, and add overrides for them.
Advantage of struct-arrays, is that I get a nice 'table-like' presentation.
*/

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
    {
      logstr log;
      bool suffMana = Spell::manaCostCheck(spellType, actor, log); // 3, check enough mana.
      if (!suffMana) { return false; }
    }

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
  /* jG, nb: after pickzapdir, multinotif shouldn't be active.
  (this is why we call 'resetNotif' afterwards.)
  */
  bool bFound = false;
  int dirKey = 0;

  // FIXME - respectMultiNotif and promptForKey should be integrated!
  LogEvents::respectMultiNotif(); // Pause if we have queued messages, before prompting.
  Cuss::clear(false); // Must come after respectMultiNotif, or we'll never see msg.
  const char* keyPrompt = "Which direction?"; // was: 'zap which direction?'
  for (;!bFound;) {
    dirKey = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-zap-dir"); 

    if (dirKey == VK_ESCAPE) {
      LogEvents::log.resetNotif();
      Cuss::clearLine(0, true); // Cuss::clear(true);
      return NoDir; // false; // Cancelled zap operation.
    }
    
    switch (dirKey) {
    case 'H': case 'J': case 'K': case 'L': case 'N': case 'B': case 'U': case 'Y': bFound = true;  break;
    default: TheUI::BeepWarn(); break; // Not a DIR key.
    }      
  } // Loop until dir key.

  Cuss::clearLine(0, true);
  LogEvents::log.resetNotif();
  CPoint dir = Map::key2dir(dirKey); 
  return dir;
}

CPoint Spell::NoDir = CPoint(0,0); // 0,0

/* I still have bug where tackle expects me to be next to mob!
*/

// (DONE) Make doors!

void Spell::trySpellIdent(SpellEnum effect) {
  // todo - figure out, if/why it fails for rush/crush/tackle.

   SpellDesc& desc = spellNC(effect);
  if (desc.ident) { debstr() << "(already known)\n"; return;  }
  if (rnd::oneIn(2)) { // Maybe every use should identify automatically.. 
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

    PushFgCol fg(rowColor); // Cuss::setTxtColor(rowColor);
    
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

