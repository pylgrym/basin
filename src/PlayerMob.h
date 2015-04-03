#pragma once

#include "Mob.h"

class PlayerMob : public Mob {
public:
  PlayerMob(Map* unIntendedMap);
  ~PlayerMob();
  virtual double act(); // returns time that action requires (0 means keep doing actions/keep initiative.)
  virtual void passTime();
  void updateLight();
  Obj* findLight();

  virtual CreatureEnum ctype() const { return CR_Player; } // Consider not having this.. (instead just relying on base ctype)

  virtual std::string a_mob()  const { return "youse"; }
  virtual std::string pronoun() const { return "you"; } // "You"/"The orc".
  virtual std::string verbS() const { return ""; } // "you HIT".

  int dungLevel; // NB!, handle persist!

  int lightStrength() const { return theLightStrength;  }
  int theLightStrength; // 1 is weak, 9 is good. (examples.)
  int theLightUnits; // how much fuel left.
  int overrideLight; // 0 or 200..

  void setLightStrength(int strength, int lightUnits)  {
    if (overrideLight) {
      strength = overrideLight;
    }
    theLightStrength = strength; 
    theLightUnits = lightUnits;
  }

  virtual int digStrength();

  void dashboard();
  void dashboardMini(); // Few stats.
  void dashboardMini2(); // horizontal.

  void rollStats();

  static PlayerMob* ply;
  static int distPlyCart(CPoint p); // right-angle distance.raw/true calc.
  static int distPly(CPoint p); // raw/true calc. (square)
  static double distPlyLight(CPoint p); // light-adjusted

  static PlayerMob* createPlayer(class Map* unintendedMap);
};

