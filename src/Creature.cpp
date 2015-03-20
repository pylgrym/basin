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

MobDef mobDefs[] = {
  // FIXME - get rid of MobDef-ix having to be == CreatureEnum.
  {  0, CR_None, ".", "nothing", { "tag1", "tag2", "tag3" } }, // , {"tag1","tag2","tag3",NULL} "tag1,tag2,tag3"
  {  1, CR_Player, "@", "you" },
  {  1, CR_Kobold, "k", ". kobold", {"hum","infra","firevuln", "-firevuln", "charge", "bump", "embed", "knockback", "fireresist", "harken","bid","hold","lock"} },
  {  2, CR_Dragon, "d", ". dragon", {"breath","dragon","swipe","tail","lash","lunge", "summon"} },
  {  3, CR_FireAnt, "a", ". fire ant", { "fire","bite" } },
  {  4, CR_Jelly, "J", ". jelly", {"slime","corrode","split"} },
  {  5,CR_Eye, "e", ". floating eye", {"construct","magic"} },
  {  6,CR_Beholder, "beholder", ". beholder", {"construct","magic"} },
  {  7,CR_Gargoyle, "gargoyle", ". gargoyle", {"demon"} },
  {  8,CR_Lich, "lich", ". lich", {"undead", "grab", "hold", "paralyze", "confuse", "charm","sleep","blind","strangle"} },
  {  9,CR_Wraith, "wraith", ". wraith", {"undead"} },
  { 10,CR_Imp, "imp", ". imp", {"demon"} },
  { 11,CR_IronGolem, "irongolem", ". iron golem", {"construct","iron","hard", "bash", "stomp","squeeze","flame","melt","burn"} },
  { 12,CR_YellowSlime, "yellowslime", ". yellow slime", {"slime"} },
  { 13,CR_BlueSlime, "blueslime", ". blue slime", {"slime"} },
  { 14,CR_EarthGolem, "earthgolem", ". earth golem", {"earth","elm","golem"} },
  { 15,CR_FireSprite, "firesprite", ". fire sprite", {"fire","elm"} },
  { 16,CR_Goblin, "goblin", ". goblin" , {"hum","goblin"}},
  { 17,CR_GoblinWar, "goblinwar", ". goblin warrior" , {"hum","goblin","war"}},
  { 18,CR_NetherSnail, "nethersnail", ". nether snail" , {"snail","slime","shell"}},
  { 19,CR_ShadowSnail, "shadowsnail", ". shadow snail" , {"snail","slime","shell"}},
  { 20,CR_Rat, "rat", ". rat" , {"critter","beast"}},
  { 21,CR_Squirrel, "squirrel", ". squirrel" , {"critter","beast"}},
  { 22,CR_BlueCentipede, "bluecentipede", ". blue centipede", {"insect","lightning"} },
  { 23,CR_RedScorpion, "redscorpion", ". red scorpion" , {"poison","fire","claw","burn"}},
  { 24,CR_GreenWorms, "greenworms", ". green worms" , {"critter","split"}},
  { 25,CR_PurpleCentipede, "purplecentipede", ". purple centipede", {"insect"} },
  { 26,CR_GreenBeetle, "greenbeetle", ". green beetle" , {"insect"}},
  { 27,CR_HypnoMoth, "hypnomoth", ". hypnomoth" , {"fly","insect"}},
  { 28,CR_Sparkly, "sparkly", ". sparkly" , {"magic"}},
  { 29,CR_RottingDevourer, "rottingdevourer", ". rotting devourer" , {"undead"}},
  { 30,CR_SilverWyrm, "silverwyrm", ". silverwyrm" , {"dragon","fly"}},
  { 31,CR_BrownBat, "brownbat", ". brown bat" , {"fly","beast"}},
  { 32,CR_BlackBat, "blackbat", ". black bat" , {"fly","beast"}},
  { 33,CR_VeiledHorror, "veiledhorror", ". veiled horror" , {"undead"}},
  { 34,CR_GreyDragon, "greydragon", ". grey dragon" , {"dragon","magic","shadow","claw","breath"}},
  { 35,CR_AzureDragon, "azuredragon", ". azure dragon" , {"dragon","ice","water","claw","breath"}},
  { 36,CR_RedDragon, "reddragon", ". red dragon" , {"dragon","fire","claw","breath"}},
  { 37,CR_OrangeDragon, "orangedragon", ". orange dragon" , {"dragon","metal","claw","breath"}},
  { 38,CR_BlueDragon, "bluedragon", ". blue dragon" , {"dragon","air","lightning","claw","breath"}},
  { 39,CR_GreenDragon, "greendragon", ". green dragon", {"dragon","gas","claw","breath"} },
  { 40,CR_YellowDragon, "yellowdragon", ". yellow dragon", {"dragon","poison","claw","breath"} },
  { 41,CR_FireElemental, "fireelemental", ". fire elemental", {"elm","fire"} },
  { 42,CR_WaterElemental, "waterelemental", ". water elemental", {"elm","water","ice"} },
};

const int maxMob = (sizeof mobDefs / sizeof MobDef);

const MobDef&  Creature::mobDef(CreatureEnum type) {
  static MobDef badMob = { 0, CR_None, "�", "bad mtype" };
  if (type < 0 || type >= maxMob) { return badMob; } //  L"out of bounds, creature enum.";
  return mobDefs[type]; 
}

MobDef&  Creature::mobDefNC(CreatureEnum type) {
  static MobDef badMob = { 0, CR_None, "�", "bad mtype" };
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

void Creature::initBehaviour(MobDef& def) {
  def.minrange   = Rnd::rndC(0, 4);
  def.maxrange   = Rnd::rnd(def.minrange, 40);
  def.moralePct  = Rnd::rndC(0, 100);
  def.chargePct  = Rnd::rndC(0, 100);
  def.retreatPct = Rnd::rndC(0, 100);;
}

void funfun() {
  std::vector<int> myInts;
  for (auto i : myInts) {
  }
}
