#pragma once

#include <map>
#include <string>
#include <vector>
#include "numutil/myrnd.h"

typedef std::string ModSource;
typedef int ModDelta;
typedef std::map<ModSource, ModDelta> ModColl;


class Stat {
public:
  Stat(const std::string& name_) :name(name_) { base = 0; onusBonus = 0; roll(); }
  // Stat():name("nostat?") { v = 0; roll();  }

  int v() const { return base + onusBonus;  }
  int base; // value.

  /* onusBonus: temporary penalties,bonuses, adjustments to stat (must be made in a different way, so we can keep track of different contributions - 
  e.g. a push/pop stack of 'tags', added to a map, e.g. std::map< std::string(tag), deltaInt >
  */
  int onusBonus;
  ModColl mods; // better approach than onusBonus - named/tagged modifiers, we can find/add/remove.
  int sumMods() // const 
  { 
    int total = 0;
    ModColl::iterator i;
    for (i = mods.begin(); i != mods.end(); ++i) {
      ModDelta& md = i->second;
      total += md;
    }
    return total;
  }
   
  std::string name;
  void roll();
  int mdf() const; // Beware, I have two different mechanisms active for this..

  void makeAvg() { base = 11; }

  bool persist(class Persist& p);
};

/* I don't like this interface,
I think I'll roll a mix of named fields and enums. */
/*
enum StatsEnum { 
  STREN,
  INTEL,
  DEXTE,
  STAMI,
  WISDO,
  CHARI,
};
*/

class Stats // Actually, ability stats..
{
public:
  Stat Str, Int, Dex, Con, Wis, Chr;
  //std::map< std::string, Stat > s;
  // std::map< std::string, Stat* > s;

  Stat& stat(const char* name);

  // void addStat(const std::string& name);
  Stats(int level_, bool bPlayer_);
  ~Stats();

  void makeAvg() {
    Str.makeAvg(); Int.makeAvg(); Dex.makeAvg(); 
    Con.makeAvg(); Wis.makeAvg(); Chr.makeAvg();
  }

  bool isPlayer; // important for some calcs, because of singleton-silliness (no player stats for mobs..)
  int theLevel;
  void setLevel(int level_);
  int level() const { return theLevel;  }
  int lvlAdj(int in) const;


  void initStats();
  void calcStats();

  enum StatEnums { HitDie = 5};

  //int baseHP; 
  // std::vector<int> hp_rolls; 
  int hp;
  int maxHP;

  int xp;
  int xpTotal;
  int xpToLevel;

  int mana;
  int maxMana;

  int ac;
  int toHit;
  int baseMobAC; // a varying dice roll for mob ac, a 'delta' (to make mobs not always equal)
  int wornAC_input, wornAC_output;

  // Temp/fluctuating state: Might deserve its own sub-struct; 
  // OTOH, actually all stats might need to follow..
  int hunger;
  int confused; // counter > 0 if we are confused, 0 if not confused. 

  int gold; // Possibly this should go in equipment/inventory instead..?

  bool isConfused() const { return (confused > 0);  }

  int calcMaxHP();

  int calcTotalAC();
  int calcBaseAC();
  int mobAC(); // the AC a mob gets to have, since he doesn't have 'worn items'.

  int itemACReduc(int wornAC); // Player will earn diminished AC from items.

  int calcToHit(std::ostream& os);

  void repToHit(std::ostream& os) {
    // os << "xyz";
    calcToHit(os);
  }

  // DERIVED STATS:
  int stealth() const; // based on dex mod + level.
  int alertness() const; // based on WIS mod + level.

  void healPct(int percent);
  void healAbs(int val);


  void recoverHP() { // Randomized healing.
    int frac = (maxHP / 10)+1;
    int inst = rnd(frac / 2, frac+ frac/2);
    int newHP = hp + inst;
    if (newHP > maxHP) { newHP = maxHP;  }
    hp = newHP;
  }

  void passTime();
  void updateHunger();
  void updateConfusion();

  void showStats();

  void initXP();
  void gainKillXP(int mobLevel);
  void gainLevel();

  bool persist(class Persist& p);

  // static Stats stats;
  static int statModifyEffect[18 + 1];
  int statMod(const std::string& stat);
  static int mdf(int stat); // Alternative approach

  static int calcAvgACeffect();
};

