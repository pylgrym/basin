#pragma once

// #include <vector>
#include <map>
#include "numutil/myrnd.h"



class Levelize
{
public:
  typedef int Level; // 'LevelNum' might be a more robust name.


  enum Consts {
    MaxLevel=40,
    TimeToDefeat=10,
    MaxDice=7,
    MaxSides=12
  };

  Levelize();
  ~Levelize();


  std::map< Level, int> hpAtLevel;
  void catHP();

  std::map< Level, double> dpsAtLevel;
  void catTTD_Dps();

  /////////////////////////////////
  struct DiceSet {
    std::vector<Dice> dice;
    void addDice(Dice d) { dice.push_back(d); }
    Dice randDice();
  };

  std::map < Level, DiceSet > diceLevels;

  void catAllDice();
  void catDice(int d, int x);

  int findDpsRange(double avgRoll); // returns level with that kind of TTD.
  void assignDiceToLevel(int d, int x, int level);   // categorize a specific dice combo into levels.


  // External accessors:
  static Dice randDiceForLevel(Level L);

  static Level suggestLevel(Level outset);

  static Levelize izer;
};

