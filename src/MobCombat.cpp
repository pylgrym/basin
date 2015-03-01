#include "stdafx.h"
#include "MobCombat.h"

#include "mob.h"

MobCombat::MobCombat()
{
}


MobCombat::~MobCombat()
{
}



double AttackInf::calcHitChance() const {
  double hitRatio = hitThres / 20.0;
  return hitRatio;
}


void AttackInf::repHitChance(std::ostream& os) {
  double chance = calcHitChance();
  int percent = int(chance*100.0 + 0.5);
  //os << std::fixed << std::setw(4) << std::setprecision(2) << chance;
  os << percent;
}



bool Mob::hitTest(class Mob& adv, AttackInf& ai) { 
  // int& hitRoll, int hitBonus) { // FIXME: consider passing entire weapon-obj instead of just the hitbonus.
  ai.hitRoll = Dx(20);

  // toHit is your ability to hit,
  // your opponent's ac will counter your ability to hit.
  ai.advAC = adv.stats.ac;
  ai.finalToHit = (stats.toHit + ai.wpHitBonus);
  ai.hitThres = ai.finalToHit - ai.advAC;

  bool bHit = false;
  if (ai.hitRoll == 20) { // a 20 is automatic MISS.
    bHit = false; 
  } else if (ai.hitRoll == 1) { // a 1 is automatic HIT.
    bHit = true;
  } else {
    bHit = (ai.hitRoll <= ai.hitThres);
  }
  return bHit;
}



bool Mob::calcAttack(Obj* attackItem, class Mob& adv, AttackInf& ai, AttackSchool school, std::ostream& os) { 
  // Collect 'attack info' in an AttackInfo struct.

  //ai.school = school; // FIXME, record that..
  adv.makeAngry();

  if (isPlayer()) {  
    if (attackItem != NULL) { ai.wpHitBonus = attackItem->toHit; }  
  }

  ai.bHit = hitTest(adv, ai);  
  if (!ai.bHit) { return false;  }

  ai.attackDice = mobWeaponDice();
  if (isPlayer()) { 
    if (attackItem != NULL) {   
      ai.attackDice = attackItem->dmgDice;   
      ai.dmgBonus = attackItem->toDmg;  
    }
   
  }

  {
    logstr log; log << "attack roll " << ai.attackDice.n << "d" << ai.attackDice.x << ": ";
    ai.dmgRoll = ai.attackDice.roll(log); 
  }

  ai.dmgMod = stats.statMod("str"); // You get your strength bonus added to dmg.
  ai.dmg += ai.dmgRoll + ai.dmgMod + ai.dmgBonus;
  if (ai.dmg < 1) { ai.dmg = 1; } // You always hit for at least 1

  ai.dmgTaken = adv.takeDamage(ai.dmg, school);  

  return true;
}





int Mob::takeDamage(int dmg, AttackSchool damageType) { // returns dmgTaken
  int dmgTaken = dmg; // Might be adjusted by resistances or vulnerabilities. (should we calculate this here, or outside in dmg code?)
  stats.hp -= dmgTaken;
  return dmgTaken;
}


