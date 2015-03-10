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






MobDef mobDefs[] = {
  {  0,CR_None, ".", "nothing" },
  {  1,CR_Player, "@", "you" },
  {  1,CR_Kobold, "k", ". kobold" },
  {  2,CR_Dragon, "d", ". dragon" },
  {  3,CR_FireAnt, "a", ". fire ant" },
  {  4,CR_Jelly, "J", ". jelly" },
  {  5,CR_Eye, "e", ". floating eye" },
  {  6,CR_Beholder, "beholder", ". beholder" },
  {  7,CR_Gargoyle, "gargoyle", ". gargoyle" },
  {  8,CR_Lich, "lich", ". lich" },
  {  9,CR_Wraith, "wraith", ". wraith" },
  { 10,CR_Imp, "imp", ". imp" },
  { 11,CR_IronGolem, "irongolem", ". iron golem" },
  { 12,CR_YellowSlime, "yellowslime", ". yellow slime" },
  { 13,CR_BlueSlime, "blueslime", ". blue slime" },
  { 14,CR_EarthGolem, "earthgolem", ". earth golem" },
  { 15,CR_FireSprite, "firesprite", ". fire sprite" },
  { 16,CR_Goblin, "goblin", ". goblin" },
  { 17,CR_GoblinWar, "goblinwar", ". goblin warrior" },
  { 18,CR_NetherSnail, "nethersnail", ". nether snail" },
  { 19,CR_ShadowSnail, "shadowsnail", ". shadow snail" },
  { 20,CR_Rat, "rat", ". rat" },
  { 21,CR_Squirrel, "squirrel", ". squirrel" },
  { 22,CR_BlueCentipede, "bluecentipede", ". blue centipede" },
  { 23,CR_RedScorpion, "redscorpion", ". red scorpion" },
  { 24,CR_GreenWorms, "greenworms", ". green worms" },
  { 25,CR_PurpleCentipede, "purplecentipede", ". purple centipede" },
  { 26,CR_GreenBeetle, "greenbeetle", ". green beetle" },
  { 27,CR_HypnoMoth, "hypnomoth", ". hypnomoth" },
  { 28,CR_Sparkly, "sparkly", ". sparkly" },
  { 29,CR_RottingDevourer, "rottingdevourer", ". rotting devourer" },
  { 30,CR_SilverWyrm, "silverwyrm", ". silverwyrm" },
  { 31,CR_BrownBat, "brownbat", ". brown bat" },
  { 32,CR_BlackBat, "blackbat", ". black bat" },
  { 33,CR_VeiledHorror, "veiledhorror", ". veiled horror" },
  { 34,CR_GreyDragon, "greydragon", ". grey dragon" },
  { 35,CR_AzureDragon, "azuredragon", ". azure dragon" },
  { 36,CR_RedDragon, "reddragon", ". red dragon" },
  { 37,CR_OrangeDragon, "orangedragon", ". orange dragon" },
  { 38,CR_BlueDragon, "bluedragon", ". blue dragon" },
  { 39,CR_GreenDragon, "greendragon", ". green dragon" },
  { 40,CR_YellowDragon, "yellowdragon", ". yellow dragon" },
  { 41,CR_FireElemental, "fireelemental", ". fire elemental" },
  { 42,CR_WaterElemental, "waterelemental", ". water elemental" },
};

const int maxMob = (sizeof mobDefs / sizeof MobDef);

const MobDef&  Creature::mobDef(CreatureEnum type) {
  static MobDef badMob = { 0, CR_None, "§", "bad mtype" };
  if (type < 0 || type >= maxMob) { return badMob; } //  L"out of bounds, creature enum.";
  return mobDefs[type]; 
}

const char* Creature::ctypeAsStr(CreatureEnum type) { return mobDef(type).tilekey; }
const char* Creature::ctypeAsDesc(CreatureEnum type) { return mobDef(type).desc; }

