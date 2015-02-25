#include "stdafx.h"
#include "Stats.h"

#include "numutil/myrnd.h"

#include <sstream>

#include "util/debstr.h"

#include "LogEvents.h"

#include "cellmap/cellmap.h"

#include "cuss.h"

#include "Mob.h"

#include <assert.h>

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
:theLevel(mlevel)
,hunger(1500)
,confused(0)
,gold(0)
,xp(0)
,mana(0)
,maxMana(0)
,isPlayer(bPlayer_)
{
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

  debstr() << " level is:" << level() << "\n";
  // for (int i = 1; i < level; ++i) { } // For filling up hit points.
  initStats();
  initXP();
}


void Stats::initStats() {
  calcStats();
  hp = maxHP; // We can't do that in 'calcStats', or user would get too many hp.
}


void Stats::calcStats() {
  maxHP = calcMaxHP();
  ac = calcTotalAC();

  std::stringstream dummy;
  toHit = calcToHit(dummy);
}


void Stats::initXP() {
  const int XpLevelScale = 10;
  xpToLevel = level() * XpLevelScale;
  /* fixme: xp should be totals, not just ' to level'.
  */
}

void Stats::gainKillXP(int mobLevel) {
  logstr log; log << "You gain " << mobLevel << " xp.";
  xp += mobLevel;
  if (xp >= xpToLevel) {
    gainLevel();
  }
}

void Stats::gainLevel() {
  int remainder = xp - xpToLevel;
  theLevel += 1;
  calcStats(); // initStats();
  initXP();
  logstr log; log << "You have gained a level!";
}

int Stats::calcBaseAC() {
  // s["dex"]
  int dexMod = Dex.mdf(); // Your dex-modifier becomes (part of) your armour class (you move quickly to avoid being hit.)
  int val_ac = (level()/2) + dexMod; // You get half your level as AC contribution.
  return val_ac;
}

int Stats::calcTotalAC() {
  int base = calcBaseAC();
  int wornAC = Equ::worn.calcWornAC();
  int total = base + wornAC;
  return total;
}

int Stats::calcToHit(std::ostream& os) {
  const int globOffset = 9; // A global constant setting, what default chance should be to hit something.

  // s["str"]
  int strMod = Str.mdf(); // Your dex-modifier becomes (part of) your armour class (you move quickly to avoid being hit.)
  os << "s+" << strMod;

  int levelContrib = (level() / 2);
  os << "L+" << levelContrib;
  debstr() << level() << "/" << levelContrib << "\n";

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
  int val = Con.v + (HitDie + Con.mdf()) * level(); 
  //int val = s["sta"].v + (HitDie *level + statMod("sta")*level); // alternative, illustrating that statMod might fluctuate.
  return val;
}


Stats::~Stats()
{
}

// void Stats::addStat(const std::string& name) { s[name] = Stat(name); }


void Stat::roll() {
  // https://klubkev.org/~ksulliva/ralph/dnd-stats.html
  v = nDx(3, 6);
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
  static Stat noStat;
  return noStat;
}

int Stats::statMod(const std::string& thestat) {
  Stat& theStat = stat(thestat.c_str()); // s[stat]; // stats.
  int mod = statModifyEffect[theStat.v];
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
  return Stats::mdf(v);
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


void Stats::heal(int percent) { // May also be used negative.
  int deltaHP = hp*percent / 100;
  int newHP = hp + deltaHP;
  if (newHP > maxHP) {
    newHP = maxHP;
  }
  hp = newHP;

  logstr log;
  if (percent > 0) {
    log << "You feel your health returning.";
  } else {
    log << "Your health worsens.";
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
  s << "STR:" << this->Str.v;         pr(s);
  s << "INT:" << this->Int.v;         pr(s);
  s << "DEX:" << this->Dex.v;         pr(s);
  s << "WIS:" << this->Wis.v;         pr(s);
  s << "CHR:" << this->Chr.v;         pr(s);
  s << "CON:" << this->Con.v;         pr(s);

  int lightStr = PlayerMob::ply->lightStrength();
  int lightUnits = PlayerMob::ply->theLightUnits;
  s << "Light:" << lightStr;          
  s << ", fuel left:" << lightUnits;          
  pr(s);
  // Cuss::prtL(s.str().c_str());  
}



bool Stats::persist(Persist& p) {
  Str.persist(p);
  Int.persist(p);
  Dex.persist(p);
  Con.persist(p);
  Wis.persist(p);
  Chr.persist(p);


  p.transfer(theLevel,  "level"); //p.os << this->theLevel;
  p.transfer(maxHP,     "maxHP"); //p.os << this->maxHP;
  p.transfer(hp,        "hp"); //p.os << this->hp;
  p.transfer(xp,        "xp"); //p.os << this->xp;
  p.transfer(xpToLevel, "xpToLevel"); //p.os << this->xpToLevel;
  p.transfer(ac,        "ac"); //p.os << this->ac;
  p.transfer(toHit,     "toHit"); //p.os << this->toHit;
  p.transfer(hunger,    "hunger"); //p.os << this->hunger;
  p.transfer(confused,  "confused"); //p.os << this->confused;
  p.transfer(gold,      "gold"); //p.os << this->gold;
  return true;
}


bool Stat::persist(Persist& p) {
  p.os << this->v;
  return true;
}
