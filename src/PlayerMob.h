#pragma once

#include "Mob.h"

class PlayerMob : public Mob {
public:
  PlayerMob();
  ~PlayerMob();
  virtual double act(); // returns time that action requires (0 means keep doing actions/keep initiative.)
  virtual void passTime();
  void updateLight();
  Obj* findLight();

  virtual CreatureEnum ctype() const { return CR_Player; } // Consider not having this.. (instead just relying on base ctype)

  virtual std::string Mob::a_mob()  const { return "youse"; }
  virtual std::string pronoun() const { return "you"; } // "You"/"The orc".
  virtual std::string verbS() const { return ""; } // "you HIT".

  int dungLevel; // NB!, handle persist!

  int lightStrength() const { return theLightStrength;  }
  int theLightStrength; // 1 is weak, 9 is good. (examples.)
  int theLightUnits; // how much fuel left.

  void setLightStrength(int strength, int lightUnits)  { 
    theLightStrength = strength; 
    theLightUnits = lightUnits;
  }

  virtual int digStrength();

  void dashboard();

  static PlayerMob* ply;
  static int distPly(CPoint p); // raw/true calc. (square)
  static int distPlyLight(CPoint p); // light-adjusted

  static PlayerMob* createPlayer();
};

