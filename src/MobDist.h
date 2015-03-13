#pragma once

#include "Creature.h"

const int MaxLevel = 40;
typedef double RatingNum;

class MobRating {
public:
  MobRating() { rating = 0.0; mobIx = CR_None; }
  RatingNum rating;
  CreatureEnum mobIx;

  bool operator < (const MobRating& rhs) const;
};


class MobDist
{
public:
  MobDist();
  ~MobDist();

  static void enumerate();
  static void enumTownLevel();
  static void dump();

  static CreatureEnum suggRndMob(int level);

};

