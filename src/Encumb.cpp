#include "stdafx.h"
#include "Encumb.h"

// Consider: If equipment and inventory was added to the mob/player,
// then we could avoid the singleton issue.(however stats and inv need to see each other somehow.)

#include "Stats.h"
#include "Equ.h"
#include "Bag.h"
#include "Mob.h"

double Encumb::totalWeight() {
  double bagWeight = Bag::bag.bagWeight();
  double wornWeight = Equ::worn.wornWeight();
  double totalWeight = wornWeight + bagWeight;
  return totalWeight;
}

Encumb::EncumbEnum Encumb::enc() {
  double kilos = totalWeight();
  Stats& stats = PlayerMob::ply->stats;
  EncumbEnum encumbrance = calcEnc(kilos, stats);
  return encumbrance;
}


Encumb::EncumbEnum Encumb::calcEnc(double kilos, Stats& stats) {
  // Strength 10 can carry 20,40,60 kilos.
  // max 20 'light', max 40 'medium', max 60 'heavy', >60 = can't lift.
  // We take str 10 *2, *4, *6.
  int lightEnc = stats.Str.v() * 2;
  if (kilos <= lightEnc) { return LightE; }

  int mediumEnc = stats.Str.v() * 4;
  if (kilos <= mediumEnc) { return MediumE; }

  int heavyEnc = stats.Str.v() * 6;
  if (kilos <= heavyEnc) { return HeavyE; }

  return CantLiftE;
}


const char* Encumb::encTxt(EncumbEnum type) {
  switch (type) {
  case LightE:    return ""; 
  case MediumE:   return "Heavy"; 
  case HeavyE:    return "Unwieldy";
  case CantLiftE: return "Too heavy";
  }
  return "errEncumb";
}



Encumb::Encumb()
{
}


Encumb::~Encumb()
{
}
