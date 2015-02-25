#pragma once

#include <map>
#include <string>
#include <vector>
#include "numutil/myrnd.h"

class Stat {
public:
  Stat(const std::string& name_):name(name_) { v = 0; roll();  }
  Stat():name("nostat?") { v = 0; roll();  }

  int v; // value.
  std::string name;
  void roll();
  int mdf() const; // Beware, I have two different mechanisms active for this..
};

/* I don't like this interface,
I think I'll roll a mix of named fields and enums. */
enum StatsEnum { 
  STREN,
  INTEL,
  DEXTE,
  STAMI,
  WISDO,
  CHARI,
};

class Stats // Actually, ability stats..
{
public:
  Stat Str, Int, Dex, Con, Wis, Chr;
  //std::map< std::string, Stat > s;
  // std::map< std::string, Stat* > s;

  Stat& stat(const char* name);

  // void addStat(const std::string& name);
  Stats(int level_);
  ~Stats();

  int theLevel;
  void setLevel(int level_);
  int level() const { return theLevel;  }

  void initStats();
  void calcStats();

  enum StatEnums { HitDie = 5};
  int maxHP;
  int hp;
  int xp;
  int xpToLevel;
  //int baseHP; 
  // std::vector<int> hp_rolls; 
  int ac;
  int toHit;

  // Temp/fluctuating state: Might deserve its own sub-struct; 
  // OTOH, actually all stats might need to follow..
  int hunger;
  int confused; // counter > 0 if we are confused, 0 if not confused. 

  int gold; // Possibly this should go in equipment/inventory instead..?

  bool isConfused() const { return (confused > 0);  }

  int calcMaxHP();
  int calcTotalAC();
  int calcBaseAC();

  int calcToHit(std::ostream& os);

  void repToHit(std::ostream& os) {
    // os << "xyz";
    calcToHit(os);
  }

  void heal(int percent);

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

  // static Stats stats;
  static int statModifyEffect[18 + 1];
  int statMod(const std::string& stat);
  static int mdf(int stat); // Alternative approach


};

