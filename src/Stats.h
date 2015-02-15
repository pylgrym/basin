#pragma once

#include <map>
#include <string>
#include <vector>

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
  std::map< std::string, Stat > s;
  void addStat(const std::string& name);
  Stats();
  ~Stats();

  int theLevel;
  void setLevel(int level_);
  int level() const { return theLevel;  }

  enum StatEnums { HitDie = 5};
  int maxHP;
  int hp;
  //int baseHP; 
  // std::vector<int> hp_rolls; 
  int ac;
  int toHit;

  int calcMaxHP();
  int calcAC();
  int calcToHit(std::ostream& os);

  void repToHit(std::ostream& os) {
    // os << "xyz";
    calcToHit(os);
  }


  // static Stats stats;
  static int statModifyEffect[18 + 1];
  int statMod(const std::string& stat);
  static int mdf(int stat); // Alternative approach


};

