#include "stdafx.h"
#include "MobDist.h"
#include <set>
#include "numutil/myrnd.h"
#include <assert.h>
#include <iomanip>


MobDist::MobDist()
{
}


MobDist::~MobDist()
{
}



bool MobRating::operator < (const MobRating& rhs) const {
  if (rating != rhs.rating) { return rating < rhs.rating; }
  return mobIx < rhs.mobIx;
}



class LevelRating {
public:
  LevelRating() { ratingSum = 0.0;  }
  std::set< MobRating > mobs;
  double ratingSum;

  void addRating(const MobRating& r) { mobs.insert(r);  }

  void makeLevelSum() {
    ratingSum = 0.0;
    std::set< MobRating >::iterator i;
    for (i = mobs.begin(); i != mobs.end(); ++i) {
      ratingSum += (*i).rating;
    }
  }

  CreatureEnum rndMob() {
    assert(ratingSum != 0.0); // Means level has not been initialized.
    double rndChoice = rnd( (int) ratingSum);
    double accum = 0.0;
    std::set< MobRating >::iterator i;
    for (i = mobs.begin(); i != mobs.end(); ++i) {
      accum += (*i).rating;
      if (rndChoice < accum) {
        return (*i).mobIx;
      }
    }
    return (CreatureEnum) (CR_MaxLimit - 1);
  }

  double mobRating(CreatureEnum mobIx) {
    std::set< MobRating >::iterator i;
    for (i = mobs.begin(); i != mobs.end(); ++i) {
      const MobRating& mr = *i;
      if (mr.mobIx == mobIx) {
        double rate = mr.rating / ratingSum;
        return rate;
      }
    }
    return 0.0;
  }

} ratings[MaxLevel + 1];


CreatureEnum MobDist::suggRndMob(int levelNum) {
  assert(levelNum >= 0);
  assert(levelNum <= MaxLevel);
  LevelRating& level = ratings[levelNum];
  CreatureEnum ctype = level.rndMob();
  return ctype;
}

void MobDist::dump() {
  std::ofstream os("mobstats.txt");

  for (int i = CR_Kobold; i < CR_MaxLimit; ++i) { // We'll make rows for each mobb.
    MobRating mr; mr.mobIx = (CreatureEnum)i;
    const MobDef& def = Creature::mobDef(mr.mobIx);
    os << def.desc << ";";

    for (int L = 1; L < MaxLevel; ++L) { // Go through all levels.
      LevelRating& level = ratings[L];
      double rate = level.mobRating(mr.mobIx);
      int intRate = int(rate * 1000.0 + 0.5);
      double rounded = (intRate / 10.0);
      if (rounded == 0.0) {
        os << "-;";
      } else {
        os << std::fixed << std::setprecision(1) << rounded << ";";
      }
    }
    os << std::endl;
  }
}

void MobDist::enumTownLevel() {
  LevelRating& level = ratings[0];
  MobRating mr; 

  // Add fixed town level mobs:
  mr.mobIx = CR_Kobold;   mr.rating = 10; level.addRating(mr);
  mr.mobIx = CR_Rat;      mr.rating = 10; level.addRating(mr);
  mr.mobIx = CR_Squirrel; mr.rating = 10; level.addRating(mr);
  mr.mobIx = CR_Goblin;   mr.rating = 10; level.addRating(mr);

  level.makeLevelSum();
}

void MobDist::enumerate() {
  const RatingNum MaxRating = 1000.0;

  enumTownLevel();

  for (int L = 1; L < MaxLevel; ++L) { // Go through all levels.
    LevelRating& level = ratings[L];
    for (int i = CR_Kobold; i < CR_MaxLimit; ++i) {
      MobRating mr; mr.mobIx = (CreatureEnum)i; 
      const MobDef& def = Creature::mobDef(mr.mobIx);

      int deltaL = (L - def.mlevel); //  '+2' means 'mob is 2 levels lower' (and in the process of (slowly) fading out.)
      //                             //, '-2' means 'mob is 2 above cur level' (and in the process of (abruptly) fading in.
      mr.rating = 0;
      if (deltaL >= 0) { // Above current level, we slowly fade out.
        const int softFadeIn = 2; // Change this from zero to e.g. 2, to not fade in/out so abruptly (zero means 1000, 500,333,250.)
        mr.rating = MaxRating / (deltaL + 1 + softFadeIn);
      } else { // ( <0 ) // Below current level, we abruptly fade in (ie squared.)
        int absDelta = -deltaL;
        int squareDelta = (absDelta + 1); squareDelta = (squareDelta * squareDelta);
        mr.rating = MaxRating / squareDelta;

        const int hardFloor = -3; // We never allow high-level monsters more than 3 levels under intended level.
        if (deltaL < hardFloor) { // (IE "no multihued ancient dragons on L1.")
          mr.rating = 0; // Force it to zero.
        }
      }
      // Now add this mob rating to cur Level.
      if (mr.rating > 0) {
        level.addRating(mr);
      }

    } // for mobs.
    level.makeLevelSum();
  } // for levels.

  dump();
}
