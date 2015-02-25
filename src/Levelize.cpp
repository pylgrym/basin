#include "stdafx.h"
#include "Levelize.h"

#include "Stats.h"

Levelize::Levelize()
{
  catHP();
  catTTD_Dps(); // Depends on hpAtLevel from catHP.
  catAllDice();
}

Levelize Levelize::izer;


Levelize::~Levelize()
{
}


void Levelize::catHP() { // categorize HP into levels.
  Stats stats(1,false); // not a player mob..
  stats.Con.v = 11; // Middlish no-modifier stat.
  for (Level L = 0; L < MaxLevel; ++L) {
    stats.setLevel(L);
    stats.calcStats();
    hpAtLevel[L] = stats.maxHP;
  }
}


void Levelize::catTTD_Dps() { // categorize timeToDefeat/Damage-Per-'Slash' into levels.
  for (Level L = 0; L < MaxLevel; ++L) {
    double hp = hpAtLevel[L];
    dpsAtLevel[L] = hp / TimeToDefeat;
  }
}



void Levelize::catAllDice() { // categorize 'all' dice combos into levels.
  for (int d = 1; d < MaxDice; ++d) {
    for (int x = 2; x < MaxSides; ++x) {
      catDice(d, x);
    }
  }
}

void Levelize::catDice(int d, int x) { // categorize a specific dice combo into levels.
  double avgRoll = d*(x + 1.0) / 2.0;
  /* The avgroll-damage must be mapped into dpsAtLevel ranges.*/
  Level level = findDpsRange(avgRoll);
  assignDiceToLevel(d, x, level);
}

Levelize::Level Levelize::findDpsRange(double avgRoll) {
  /* returns level where ttd-dps is larger than avgRoll.
   (ie avgRoll is too high dmg for previous level.)
  */
  for (Level L = 0; L < MaxLevel; ++L) {
    double dmgAtLevel = dpsAtLevel[L];
    if (dmgAtLevel > avgRoll) { return L; }
  }
  return MaxLevel;
}


void Levelize::assignDiceToLevel(int d, int x, Level level) {   // categorize a specific dice combo into levels.
  diceLevels[level].addDice( Dice(d, x));
}


Dice Levelize::randDiceForLevel(Level L) {
  DiceSet& ds = izer.diceLevels[L];
  Dice dice = ds.randDice();
  return dice;
}


Dice Levelize::DiceSet::randDice() { // Pick a random choice from the set:
  Dice choice(1, 4);
  if (dice.size() > 0) {
    int ix = rnd(dice.size());
    choice = dice[ix];
  }
  return choice;
}


Levelize::Level Levelize::suggestLevel(Level outset) {
  /* will suggest a randomized 'appropriate' level based on 'level outset'.
  IE mostly you get same level, 
  and with reduced probab, you get higher levels.

  With 33%, you get 
  66% same, 
  interesting things happen between 33 and 50 (50 gives too many highlevel things.)
  */

  const int limitPct = 38; // 33;
  int delta = -1;
  for (int roll = 0; (roll < limitPct); ++delta, roll = rnd(100)) { }
  return outset + delta;
}
