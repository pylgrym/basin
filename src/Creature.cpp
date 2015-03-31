#include "stdafx.h"
#include "Creature.h"

#include <vector>

#include "Mob.h"
#include "sprites/Tilemap.h"

bool Creature::isBlockingCreature(CreatureEnum type) {
  switch (type) {
  case CR_None:
    return false; // not blocking.
    // everything else is blocking movement..
  }

  return true;
}


CreatureEnum Creature::type() const {
  return m != NULL ? m->ctype() : CR_None;
}



/* tag design:
I consider enums instead of strings, which would aid in spellchecking/verifying 
that tags exist/are handled correctly.
 And it would allow minus, e.g. fireresist/minus fire.
I need to have sensible defaults, so I don't have to repeatedly specify tons of 'default' values.
Also, I want certain general classes to specify clusters of attributes, e.g. 'dragon'.
e.g. dragon means i dont' have to re-specify breath too. - dragon implies breath.
  i don't want uppercase or dashes-underscores in my tag names.
  i might change mind, if tags can somehow combine.

in monster manual, i saw different classes have e.g. different 'hit dice', ie sources of hitpoints
 - eg. dragons would have lots.
 similarly, some mob classes would have high armour or dex or strength or speed/stealth.
 i want 'elemental schools', that might oppose each other - 
Fire,flame,burn opposite water-frost-ice.
ice-fire-earth-air
ice-fire-rock-air
*/

/* attack-behaviours:  
 - slime will leave/fill a slime-square, that is hurtful to step on.
Stepping on it would dot-damage you, and/or put a temp-state-bad on you, e.g. 'slimed' with some effect.


(dmg-time)'DOT' spells, would hurt you in chunks,
possibly of different sizes, suggesting you try to 'counter'/reduce them somehow
(e.g. have an item with a turn-using use-effect, that will counter
 - better than just having a 'cancel-out' reverse-item (where you never interact with the effect.)

  - breath-attack:  use Bresenham to draw a narrow cone of 'breath'- fire, frost etc.
Possibly the smoke will last a few turns?

 - DONE: summon-monster, 
 - summon-undead, summon-demon.

alter between different states - 'the golem starts glowing/growing, shrinks'?
(when it's in the bad mode, stay away from it..)

 - charge: 'jumps' directly to player; requires LoS, possibly hurts.
 - grab/hold/grip: player can still hit/use spells, but he can't move away from mob (because it's gripping him.)
 - shove/bump: hits player so hard, he's knocked one or more squares back.
 - crush(word?) hurting player hard by squeezing him between wall and mob - depends on wall behind you
 - (pinning?)
 (so if you avoid standing next to walls, not possible/keep circling. maybe only works on NSEW?)
 - 'embed' knock you INTO the rock/wall, making a hole for you.
 - throw a monster at you?
 - hook/grasp: holds on to you, so that you drag the monster along with you, when you move?
 - similarly a dragging move where the monster drags you with it.
 - engulf
 - spit.
 - lava ball or something - something that sets your environment on fire.
 - things that glow/burn/seep/acid for a bit in your environment.
 - teleport, both ways - itself to you, and you to it, and a 3rd mob to you. or you to 3rd mob.
 - we need both teleport-other and teleport-self; 
 start with teleport-self, make it teleport-other.
*/

/*
humanoid, beast. monstrosity-monster.fey,plant undead fiend demon ghost elemental dragon-kin. giant aberration. construct. ooze.
demon devil  fiend.

tele phase and slow, annoying bugger. slowstinger.
more kinds of terrain.
*/

// design: abilities are just 3rd-person-verbs, so they can be used as-is in texts.
MobDef mobDefs[] = {
  // FIXME - get rid of MobDef-ix having to be == CreatureEnum.
  { 0, CR_None, ".", "nothing", {},  { "tag1", "tag2", "tag3" } }, // , {"tag1","tag2","tag3",NULL} "tag1,tag2,tag3"
  { 1, CR_Player, "@", "you", {}, },
  { 1, CR_Kobold, "k", ". kobold", { "stabs", "lunges", "hum", "infra", "firevuln", "-firevuln", "charges", "bumps", "embeds", "knocks back", "fireresist", "harkens", "bids", "holds", "locks" }, { "hum" }},
  { 2, CR_Dragon, "d", ". dragon", { "breathes", "dragon", "swipes", "tail", "lashes", "lunges", "summons" }, {"dragon"}},
  { 3, CR_FireAnt, "a", ". fire ant", { "spits", "hisses", "fire", "clacks","bites" }, { "beast" } },
  { 4, CR_Jelly, "J", ". jelly", { "slimes", "corrodes", "splits" }, { "aber" }},
  { 5, CR_Eye, "e", ". floating eye", { "gazes", "bores", "stares", "swirls" }, { "construct", "magic" } },
  { 6, CR_Beholder, "beholder", ". beholder", { "gazes", "bores", "swirls" }, { "construct", "magic" } },
  { 7, CR_Gargoyle, "gargoyle", ". gargoyle", { "claws", "bites", "lashes" }, { "demon","evil"}},
  { 8, CR_Lich, "lich", ". lich", { "undead", "grabs", "holds", "murmurs", "paralyzes", "confuses", "charms", "sleeps", "blinds", "strangles" },{"undead"}},
  { 9, CR_Wraith, "wraith", ". wraith", {"wails","haunts","fears","whispers","screams","scares","frightens","dooms"},{"undead"}},
  { 10,CR_Imp, "imp", ". imp", {"cackle", "leers","curses","gestures","mumbles","chants","snickers","claws","scratches"},{"demon"}},
  { 11,CR_IronGolem, "irongolem", ". iron golem", {"construct","iron","hard", "bashes", "stomps","squeezes","flame","melts","burns"}},
  { 12,CR_YellowSlime, "yellowslime", ". yellow slime", {"corrodes","slimes","oozes","bites"}, {"slime"}},
  { 13,CR_BlueSlime, "blueslime", ". blue slime", {"oozes","dribbles","bites"},  {"slime"}},
  { 14,CR_EarthGolem, "earthgolem", ". earth golem", {"stomps","bashes","quakes","bites"},  {"earth","elm","golem"}},
  { 15,CR_FireSprite, "firesprite", ". fire sprite", {"flames","lashes","burns","bites"},  {"fire","elm"}},
  { 16,CR_Goblin, "goblin", ". goblin" , {"stabs","lunges","shoots","bites"},  {"hum","goblin"}},
  { 17,CR_GoblinWar, "goblinwar", ". goblin warrior" , {"cleaves","slashes","lunges","bites"},  {"hum","goblin","war"}},
  { 18,CR_NetherSnail, "nethersnail", ". nether snail" , {"drains","oozes","slimes","dribbles","bites"},  {"snail","slime","shell"}},
  { 19,CR_ShadowSnail, "shadowsnail", ". shadow snail" , {"slimes","bites"},  {"snail","slime","shell"}},
  { 20,CR_Rat, "rat", ". rat" , {"claws","scratches","gnaws","bites"},  {"critter","beast"}},
  { 21,CR_Squirrel, "squirrel", ". squirrel" ,  {"scratches","bites"}, {"critter","beast"}},
  { 22,CR_BlueCentipede, "bluecentipede", ". blue centipede", {"gnaws","rattles","scurries","scuttles","bites"},  {"insect","lightning"}},
  { 23,CR_RedScorpion, "redscorpion", ". red scorpion" , {"claws","stings","bites"},  {"poison","fire","claw","burn"}},
  { 24,CR_GreenWorms, "greenworms", ". green worms" , {"corrodes","poisons","squirms","bites"},  {"critter","split"}},
  { 25,CR_PurpleCentipede, "purplecentipede", ". purple centipede", {"strangles","squeezes","bites"},  {"insect"}},
  { 26,CR_GreenBeetle, "greenbeetle", ". green beetle" , {"bashes","bumps","impales","bites"},  {"insect"}},
  { 27,CR_HypnoMoth, "hypnomoth", ". hypnomoth" , {"dusts","flutters","cackles","swirls","bites"},  {"fly","insect"}},
  { 28,CR_Sparkly, "sparkly", ". sparkly" , {"stings","bites"},  {"magic"}},
  { 29,CR_RottingDevourer, "rottingdevourer", ". rotting devourer" , {"coughs", "sputters","defiles","corrupts","rots","bites"},  {"undead"}},
  { 30,CR_SilverWyrm, "silverwyrm", ". silverwyrm" , {"pinches","claws","rakes","slithers","bites"},  {"dragon","fly"}},
  { 31,CR_BrownBat, "brownbat", ". brown bat" , {"claws","pecks","hacks","bites"},  {"fly","beast"}},
  { 32,CR_BlackBat, "blackbat", ". black bat" , {"leeches","sucks","bites"},  {"fly","beast"}},
  { 33,CR_VeiledHorror, "veiledhorror", ". veiled horror" , {"strangles","chokes","bites"},  {"undead"}},
  { 34,CR_GreyDragon, "greydragon", ". grey dragon" , {"breathes","claws","bites"},  {"dragon","magic","shadow","claw","breath"}},
  { 35,CR_AzureDragon, "azuredragon", ". azure dragon" , {"swipes","bashes","lashes","squeezes","bites"},  {"dragon","ice","water","claw","breath"}},
  { 36,CR_RedDragon, "reddragon", ". red dragon" , {"burns","rakes","lashes","flames","roasts","bites"},  {"dragon","fire","claw","breath"}},
  { 37,CR_OrangeDragon, "orangedragon", ". orange dragon" , {"confuses","charms","drains","bites"},  {"dragon","metal","claw","breath"}},
  { 38,CR_BlueDragon, "bluedragon", ". blue dragon" , {"ices","freezes","lashes","swipes","claws","bites"},  {"dragon","air","lightning","claw","breath"}},
  { 39,CR_GreenDragon, "greendragon", ". green dragon", {"stings","hurls","venoms","poisons","breathes", "spits","exhales","bites"},  {"dragon","gas","claw","breath"}},
  { 40,CR_YellowDragon, "yellowdragon", ". yellow dragon", {"quakes","squeezes","mashes","bites"},  {"dragon","poison","claw","breath"}},
  { 41,CR_FireElemental, "fireelemental", ". fire elemental", {"melts","incinerates","ablazes","flames","grills","bakes","roasts","burns","bites"},  {"elm","fire"}},
  { 42,CR_WaterElemental, "waterelemental", ". water elemental", {"splashes","flushes","chokes","drowns","bites"},  {"elm","water","ice"}},
};

const int maxMob = (sizeof mobDefs / sizeof MobDef);

const MobDef&  Creature::mobDef(CreatureEnum type) {
  static MobDef badMob = { 0, CR_None, "§", "bad mtype" };
  if (type < 0 || type >= maxMob) { return badMob; } //  L"out of bounds, creature enum.";
  return mobDefs[type]; 
}

MobDef&  Creature::mobDefNC(CreatureEnum type) {
  static MobDef badMob = { 0, CR_None, "§", "bad mtype" };
  if (type < 0 || type >= maxMob) { return badMob; } //  L"out of bounds, creature enum.";
  return mobDefs[type];
}

const char* Creature::ctypeAsStr(CreatureEnum type) { return mobDef(type).tilekey; }
const char* Creature::ctypeAsDesc(CreatureEnum type) { return mobDef(type).desc; }


void Creature::initMobDefs() {
  for (int i = 0; i < maxMob; ++i) {
    MobDef& def = mobDefNC((CreatureEnum) i);
    initBehaviour(def);
  }
}

void Creature::initMobTiles(Tiles& tiles) {
  for (int i = 0; i < maxMob; ++i) {
    MobDef& def = mobDefNC((CreatureEnum) i);

    CA2T ukey(def.tilekey);
    def.mtilekey = tiles.tile(CString(ukey));
  }
}


void Creature::initBehaviour(MobDef& def) {
  def.minrange   = rnd::rndC(0, 4);
  def.maxrange   = rnd::Rnd(def.minrange, 40);
  def.moralePct  = rnd::rndC(0, 100);
  def.chargePct  = rnd::rndC(0, 100);
  def.retreatPct = rnd::rndC(0, 100);;
}

void funfun() {
  std::vector<int> myInts;
  for (auto i : myInts) {
  }
}
