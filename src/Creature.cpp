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


MobDef mobDefs[] = {
  // FIXME - get rid of MobDef-ix having to be == CreatureEnum.
  {  0, CR_None, ".", "nothing", { "tag1", "tag2", "tag3" } }, // , {"tag1","tag2","tag3",NULL} "tag1,tag2,tag3"
  {  1, CR_Player, "@", "you" },
  {  1, CR_Kobold, "k", ". kobold", {"hum","infra","firevuln", "-firevuln", "fireresist"} },
  {  2, CR_Dragon, "d", ". dragon", {"breath","dragon"} },
  {  3, CR_FireAnt, "a", ". fire ant", { "fire","bite" } },
  {  4, CR_Jelly, "J", ". jelly", {"slime","corrode","split"} },
  {  5,CR_Eye, "e", ". floating eye", {"construct","magic"} },
  {  6,CR_Beholder, "beholder", ". beholder", {"construct","magic"} },
  {  7,CR_Gargoyle, "gargoyle", ". gargoyle", {"demon"} },
  {  8,CR_Lich, "lich", ". lich", {"undead", "grab", "hold", "paralyze", "confuse", "charm","sleep","blind","strangle"} },
  {  9,CR_Wraith, "wraith", ". wraith", {"undead"} },
  { 10,CR_Imp, "imp", ". imp", {"demon"} },
  { 11,CR_IronGolem, "irongolem", ". iron golem", {"construct","iron","hard", "bash", "stomp","squeeze"} },
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
  static MobDef badMob = { 0, CR_None, "§", "bad mtype" };
  if (type < 0 || type >= maxMob) { return badMob; } //  L"out of bounds, creature enum.";
  return mobDefs[type]; 
}

const char* Creature::ctypeAsStr(CreatureEnum type) { return mobDef(type).tilekey; }
const char* Creature::ctypeAsDesc(CreatureEnum type) { return mobDef(type).desc; }

