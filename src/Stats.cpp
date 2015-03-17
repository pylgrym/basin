#include "stdafx.h"
#include "Stats.h"
#include "numutil/myrnd.h"
#include <sstream>
#include "util/debstr.h"
#include "LogEvents.h"
#include "cellmap/cellmap.h"
#include "cuss.h"
#include "Mob.h"
#include "PlayerMob.h"
#include <assert.h>
#include "Encumb.h"

/* EXP Rules: http://www.monkeysushi.net/gaming/DnD/XP%20table.html

Level	Min. XP
1	0
2	1000
3	3000
4	6000
5	10000
6	15000
7	15000
8	28000
9	36000
10	45000
11	55000
12	66000
13	78000
14	91000
15	105000
16	120000
17	136000
18	153000
19	171000
20	190000

https://klubkev.org/~ksulliva/ralph/dnd-stats.html

skill check: d20 + your-skill-bonus, COMPARE to the difficulty-Class.
The roll must be HIGHER (not just equal), in that you can't roll zero,
so you always get a '1' added (lowest possible roll.)
  Possibly, 1 means failure or reduced effect..? (it might be nice, that we not always risk 5%..
It's ok to risk missing 5% on sword-swings (which there are plenty of, mostly inconsequential), 
but %5 to fall down a ladder is annoying.
  

  
*/

void Stats::setLevel(int level_) {
  theLevel = level_;
}



Stats::Stats(int mlevel, bool bPlayer_)
: Str("str")
, Int("int")
, Dex("dex")
, Con("con")
, Wis("wis")
, Chr("chr")
, isPlayer(bPlayer_)
, theLevel(mlevel)
, hp(0)
, maxHP(0)
, xp(0)
, xpTotal(0)
, xpToLevel(0)
, mana(0)
, maxMana(0)
, ac(0)
, baseMobAC(0)
, wornAC_input(0), wornAC_output(0)
, toHit(0)
, hunger(1500)
, confused(0)
, gold(0)
{
  baseMobAC = nDx(2, 2);

  // https://klubkev.org/~ksulliva/ralph/dnd-stats.html

  /*
  addStat("str"); // These are also called 'attributes'.
  addStat("int");
  addStat("dex");
  addStat("sta");
  addStat("wis");
  addStat("chr");
  */

  /* Additional rules: 
   - at least one stat must be > 13.
   - sum of modifiers must be > 0. 
   If not, re-do rolls.

   https://klubkev.org/~ksulliva/ralph/dnd-stats.html
  */

  // setLevel(rnd(1, 30)); 

  // Note a 'stats' is made
  // every turn, just for avg ac calc..
  //debstr() << " level is:" << level() << "\n";

  // for (int i = 1; i < level; ++i) { } // For filling up hit points.
  initStats();
  initXP();
}


void Stats::initStats() {
  calcStats();
  hp = maxHP; // We can't do that in 'calcStats', or user would get too many hp. / regain too often.
  mana = maxMana; // We can't do that in 'calcStats', or user would get too many mana.
}


void Stats::calcStats() {
  if (isPlayer) { // NB! Beware of SEQUENCE of dependent-effects, so you don't get recursive stat-feedback..
    // Right now, encumbrance is punishing - you get your stats reduced by -1, -4, -10!
    Encumb::EncumbEnum enc = Encumb::enc();
    Dex.onusBonus = -enc; // dex-AC will suffer.
    Str.onusBonus = -enc; // toHit will suffer.
  }

  maxHP = calcMaxHP();
  maxMana = calcMaxMana();
  ac = calcTotalAC();

  std::stringstream dummy;
  toHit = calcToHit(dummy);

}


void Stats::initXP() {
  const int XpLevelScale = 10;

  int remainder = xp - xpToLevel;
  xp = remainder; // I reset it, because it's a 'level-local counter'.
  xpToLevel = level() * XpLevelScale;
  /* consider: xp should be totals, not just 'to level'.
  */
}

void Stats::gainKillXP(int mobLevel) {
  logstr log; log << "You gain " << mobLevel << " xp.";
  int xpGain = mobLevel;
  xp += xpGain;
  xpTotal += xpGain;

  if (xp >= xpToLevel) {
    gainLevel();
  }
}

void Stats::gainLevel() {
  theLevel += 1;
  calcStats(); 
  initXP();
  logstr log; log << "You have gained a level!";
}



int Stats::calcBaseAC() {
  // s["dex"]
  int dexMod = Dex.mdf(); // Your dex-modifier becomes (part of) your armour class (you move quickly to avoid being hit.)
  int val_ac = (level()/2) + dexMod; // You get half your level as AC contribution.
  return val_ac;
}

int Stats::mobAC() {
  return level() + 2 + baseMobAC;  // This is too much, I think..?
}



int Stats::itemACReduc(int wornAC) {
  int actualAC = 0;

  int acCost = 1;
  int count = 0;
  for (int i = 0; i < wornAC; ++i) {
    ++count;
    if (count == acCost) {
      ++actualAC; // We earned one.
      ++acCost; // Next one will cost more.
      count = 0;
    }
  }
  return actualAC;
}



int Stats::calcTotalAC() {
  int base = calcBaseAC();
  int wornAC = 0;
  if (isPlayer) {
    wornAC = Equ::worn.calcWornAC();
    wornAC_input = wornAC;
    wornAC = itemACReduc(wornAC);
    wornAC_output = wornAC;
  } else {
    wornAC = mobAC();
  }
  int total = base + wornAC;
  return total;
}



int Stats::calcToHit(std::ostream& os) {
  const int globOffset = 18; // 9; // A global constant, what default chance should be to hit something.
  /* JG: 9 gives '50-50'. But actually, I want 'unarmoured' to be more hittable, like 90%..
  So instead, I should give mobs more armour!
  */

  // s["str"]
  int strMod = Str.mdf(); // Your dex-modifier becomes (part of) your armour class (you move quickly to avoid being hit.)
  os << "s+" << strMod;

  int levelContrib = (level() / 2);
  os << "L+" << levelContrib;

  // debstr() << "L:" << level() << "/LC:" << levelContrib << "\n";

  int val_hit = globOffset + levelContrib + strMod; // You get half your level as AC contribution.
  return val_hit;
}



int Stats::calcMaxHP() {
  /* maximum hitpoints,
  is based on something like
   - you get an offset equal to your constitution/stamina
   - for each level, you get 6+ your stamina modifier.
   -  the '6' should probably be a fixed roll of hit die.
  */
  // s["sta"]
  int val = Con.v() + (HitDie + Con.mdf()) * level(); 
  //int val = s["sta"].v + (HitDie *level + statMod("sta")*level); // alternative, illustrating that statMod might fluctuate.
  return val;
}


int fixedRoll(int dSide, int promille) {
  int roll = (dSide * promille / 1000) + 1;
  return roll;
}


int Stats::calcMaxMana() { 
  if (isPlayer) {
    int q = 42; // breakpoint.
  }
  /* maximum mana,
  is based on something like
   - you get an offset equal to your int
   - for each level, you get 2 or your int modifier.
   -  the '2' should probably be a fixed roll of hit die.
  */
  //int val = Int.v() + (HitDie + Int.mdf()) * level(); 
  //int val = s["sta"].v + (HitDie *level + statMod("sta")*level); // alternative, illustrating that statMod might fluctuate.

  int base = (Int.v() - 7); // below 8, you get nothing.
  if (base < 0) { base = 0; } 
  // so, 18 gives us '11'. But we should get 3-4.
  int L1mana = base / 3; // will give 18 a 3,'almost 4'.
  // We want a high-level to get 90 mana, and 'high' is 40, so we need 2 mana every level. You get '1dx'(your modifier.)

  // These are fixed from start - they must be persisted!
  if (manaRolls.empty() || manaRolls[1] == 0) {
    for (int i = 0; i < 40; ++i) {
      manaRolls.push_back(rnd(1000));
    }
  }

  int growth = 0;
  int manaMDF = Int.mdf(); if (manaMDF < 1) { manaMDF = 1;  } // always positive.
  for (int L = 2; L <= level(); ++L) { // add all rolls, adjusted for int-modifier.
    int manaPart = fixedRoll(manaMDF, manaRolls[L]);
    growth += manaPart;
  }

  int val = (L1mana + growth);
  return val;
}


Stats::~Stats()
{
}

// void Stats::addStat(const std::string& name) { s[name] = Stat(name); }


void Stat::roll() {
  // https://klubkev.org/~ksulliva/ralph/dnd-stats.html
  base = nDx(3, 6);
  // JG: Version 2 - instead, roll 4d6, and pick highest 3 (or rather, throw away minimum.)
}


// Stats Stats::stats;

/*
Stat 	Modifier 	Probability
(3d6) 	Probability
(4d6 drop 1)
3 	-4 	0.5% 	0.1%
4 	-3 	1.4% 	0.3%
5 	-3 	2.8% 	0.8%
6 	-2 	4.6% 	1.6%
7 	-2 	6.9% 	2.9%
8 	-1 	9.7% 	4.8%
9 	-1 	11.6% 	7.0%
10 	+0 	12.5% 	9.4%
11 	+0 	12.5% 	11.4%
12 	+1 	11.6% 	12.9%
13 	+1 	9.7% 	13.3%
14 	+2 	6.9% 	12.3%
15 	+2 	4.6% 	10.1%
16 	+3 	2.8% 	7.3%
17 	+3 	1.4% 	4.2%
18 	+4 	0.5% 	1.6%
*/

int Stats::statModifyEffect[18 + 1] = { // Only use this indirectly in functions with boundary checking.
  -4, //   0 // NOT USED
  -4, //   1 // NOT USED
  -4, //   2 // NOT USED
 	-4, //   3 	 0.5%  0.1%
 	-3, //   4 	 1.4%  0.3%
 	-3, //   5 	 2.8%  0.8%
 	-2, //   6 	 4.6%  1.6%
 	-2, //   7 	 6.9%  2.9%
 	-1, //   8 	 9.7%  4.8%
 	-1, //   9 	11.6%  7.0%
 	+0, //  10 	12.5%  9.4%
 	+0, //  11 	12.5% 11.4%
 	+1, //  12 	11.6% 12.9%
 	+1, //  13 	 9.7% 13.3%
 	+2, //  14 	 6.9% 12.3%
 	+2, //  15 	 4.6% 10.1%
 	+3, //  16 	 2.8%  7.3%
 	+3, //  17 	 1.4%  4.2%
 	+4, //  18   0.5%  1.6%
};


int Stats::mdf(int stat) { // Alternative approach, just calc it.
  int modif = ((stat - 10) / 2); // floor() - you dont even have do to floor (check negative numbers.)
  return modif;
}

Stat& Stats::stat(const char* name) {
  std::string sn = name;
  if (sn == "str") { return Str;  }
  if (sn == "int") { return Int;  }
  if (sn == "dex") { return Dex;  }
  if (sn == "wis") { return Wis;  }
  if (sn == "con") { return Con;  }
  if (sn == "chr") { return Chr;  }
  assert(false); // bad stat name. // 'sta' is not a stat.
  static Stat noStat("noStat");
  return noStat;
}

int Stats::statMod(const std::string& thestat) {
  Stat& theStat = stat(thestat.c_str()); // s[stat]; // stats.
  int mod = statModifyEffect[theStat.v()];
  return mod;
  /* Conclusion - stats cause bonus modifiers [-4;4] on other things.
  */
}


/* Weapon damage: http://www.highprogrammer.com/alan/gaming/dnd/statistics/weapondamage/
http://www.highprogrammer.com/alan/gaming/dnd/statistics/weapondamage/

Overview:
http://www.monkeysushi.net/gaming/DnD/math.html


*/



int Stat::mdf() const {
  return Stats::mdf(v());
}



void Stats::passTime() {
  /* JG: Deal with temporary effects, ie positive and negative buffs,
  and cooldowns. Possibly this is not the proper place to handle all than.
  OTOH, a lot of them might affect stats.
  */

  ac = calcTotalAC(); // Don't update all stats, just this one.. so far.

  updateHunger();
  updateConfusion();

  if (oneIn(100)) {
    if (oneIn(2)) {
      logstr log; log << "You feel a monster appearing.";
      CL->map.addRandomMob(CL->level);
    } else {
      logstr log; log << "You feel some item appearing.";
      CL->map.addRandomObj(CL->level);
    }
  }
}

void Stats::updateHunger() {
  --hunger;
  if (hunger < 0) {
    if (hunger % 10 == 0) {
      logstr log;
      log << "You are starving!";
    }
  }
}


void Stats::updateConfusion() {
  if (confused <= 0) { return; }  // no confusion going on.
  --confused; // Continuing the countdown.
  if (confused == 0) {
    logstr log; log << "Your confusion wears off.";
  }
}


void Stats::healPct(int percent, Mob* mob) { // May also be used negative.
  int deltaHP = hp*percent / 100;
  int newHP = hp + deltaHP;
  if (newHP > maxHP) {
    newHP = maxHP;
  }
  hp = newHP;

  logstr log;
  if (percent > 0) {
    if (isPlayer) { log << "You feel your health returning."; } 
    else          { log << mob->pronoun() << " appears healthier."; }
  } else {
    log << "p:" << this->isPlayer << "Your health worsens.";
  }
}

void Stats::healAbs(int val) { // May also be used negative.
  int newHP = hp + val;
  if (newHP > maxHP) {
    newHP = maxHP;
  }
  hp = newHP;

  logstr log;
  if (val > 0) {
    log << "p:" << this->isPlayer << "You feel your health returning.";
  }
  else {
    log << "p:" << this->isPlayer << "Your health worsens.";
  }
}


void pr(std::stringstream& ss) {
  std::string s = ss.str();
  Cuss::prtL(s.c_str());  
  ss.str("");
}

void Stats::showStats() {
  std::stringstream s;

  s << "AC:" << this->ac;             pr(s);
  s << "AU:" << this->gold;           pr(s);
  s << "Confused?" << this->confused; pr(s);
  s << "HP:" << this->hp;             pr(s);
  s << "maxHP:" << this->maxHP;       pr(s);
  s << "XP:" << this->xp;             pr(s);
  s << "XP-lvl:" << this->xpToLevel;  pr(s);
  s << "Hunger:" << this->hunger;     pr(s);
  s << "Level:" << this->level();     pr(s);
  s << "STR:" << this->Str.v() << " (" << Str.base << ")";       pr(s);
  s << "INT:" << this->Int.v();       pr(s);
  s << "DEX:" << this->Dex.v() << " (" << Dex.base << ")";       pr(s);
  s << "WIS:" << this->Wis.v();       pr(s);
  s << "CHR:" << this->Chr.v();       pr(s);
  s << "CON:" << this->Con.v();       pr(s);

  s << "stealth:" << this->stealth(); pr(s);
  s << "alertness:" << this->alertness(); pr(s);

  int lightStr = PlayerMob::ply->lightStrength();
  int lightUnits = PlayerMob::ply->theLightUnits;
  s << "Light:" << lightStr;          
  s << ", fuel left:" << lightUnits;          
  pr(s);

  int enc1 = Encumb::encLimits(Encumb::LightE);
  int enc2 = Encumb::encLimits(Encumb::MediumE);
  int enc3 = Encumb::encLimits(Encumb::HeavyE);
  //int enc4 = Encumb::encLimits(Encumb::CantLiftE);
  s << "enc.limits:" << enc1 << "/" << enc2 << "/" << enc3;   pr(s);

  // Cuss::prtL(s.str().c_str());  
}



bool Stats::persist(Persist& p) {
  Str.persist(p);
  Int.persist(p);
  Dex.persist(p);
  Con.persist(p);
  Wis.persist(p);
  Chr.persist(p);


  p.transfer(theLevel,  "level"); 
  p.transfer(maxHP,     "maxHP"); 
  p.transfer(hp,        "hp"); 

  p.transfer(maxMana,   "maxMana");
  p.transfer(mana,      "mana");

  p.transfer(xp,        "xp"); 
  p.transfer(xpTotal,   "xpTotal"); 
  p.transfer(xpToLevel, "xpToLevel"); 

  p.transfer(ac,        "ac"); 
  p.transfer(toHit,     "toHit");
  p.transfer(hunger,    "hunger"); 
  p.transfer(confused,  "confused"); 
  p.transfer(gold,      "gold"); 


  // FIXME: - these should probably be persisted with a count, instead of hardcoding 40.
  if (manaRolls.empty()) {
    manaRolls.resize(40);
  }
  for (int i = 0; i < 40; ++i) { // Hmm, are we persisting mob stats too?
    p.transfer(manaRolls[i], "manaRoll"); 
  }

  // just temporary: (is not necessary, because we save and load mana/maxMana too)
  // calcStats(); // Now we have all (?) data, we can recalc maxMana/maxHP.
  return true;
}


bool Stat::persist(Persist& p) {
  p.transfer(base, name.c_str());
  std::string onusTag = name + "_onus";
  p.transfer(onusBonus, onusTag.c_str());
  return true;
}


int Stats::calcAvgACeffect() {
  int level = PlayerMob::ply->stats.level();
  Stats avgStats(level, false); // Make stats for an avg same-level monster.
  avgStats.makeAvg();
  avgStats.calcStats();

  int mobToHit = avgStats.toHit;
  int playerAC = PlayerMob::ply->stats.ac;
  int thres = (mobToHit - playerAC);
  double hitRatio = thres / 20.0;
  double missRatio = ( 1.0 - hitRatio);
  int pct = int(missRatio*100.0 + 0.5);
  return pct;
}


int Stats::stealth() const { // based on dex mod + level.
  int base = Dex.mdf();
  int total = lvlAdj(base);
  return total;
}

int Stats::alertness() const { // based on WIS mod + level.
  int base = Wis.mdf();
  int total = lvlAdj(base);
  return total;
}


int Stats::lvlAdj(int in) const {
  int delta = level() / 2;
  int adj = in + delta;
  return adj;
}





bool Stats::useMana(int manaCost) { 
  int newMana = mana - manaCost;
  if (newMana < 0) {

    /*
    if (mob.stats.mana < Spell::manaCost(effect)) {

      // FIXME, make a general prompter helper func, that integrates all this:
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
    */

    newMana = 0;
  }

  mana = newMana;
  return true;
}



bool Stat::rollCheck(bool guifeedback) const {
  int roll = Dx(20);
  bool success = ( roll <= v() ); // A high stat is easy to roll under.
  // 1's and 20's ought to show up in gui!
  if (roll == 1) { // 1 is always low enough:
    logstr log; log << "Yes! You get it just right!"; // FIXME.. If it's 'inner workings' of something, multiple of these would be silly?
    success = true;  
  } 
  if (roll == 20) { // 20 is always a fail:
    logstr log; log << "Arrgh! You fumble..";
    success = false;  
  } 
  return success;
}
