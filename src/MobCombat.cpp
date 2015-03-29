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
  ai.hitRoll = rnd::Dx(20);

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



bool Mob::calcAttack(std::ostream& os, AttackInf& ai, Mob& adv, Obj* attackItem, AttackSchool school, SpellEnum spell, bool overrideHit) { 
  // Collect 'attack info' in an AttackInfo struct.

  //ai.school = school; // FIXME, record that..
  adv.makeAngry();

  if (isPlayer()) {  
    if (attackItem != NULL) { ai.wpHitBonus = attackItem->toHit; }  
  }

  ai.bHit = hitTest(adv, ai); 
  if (overrideHit) { ai.bHit = true; }
  if (!ai.bHit) { return false;  }

  if (spell == SP_NoSpell) { // A physical/melee attack.. (or an arrow/throw-item?)
    ai.attackDice = mobWeaponDice();
    if (isPlayer()) { 
      if (attackItem != NULL) {   
        ai.attackDice = attackItem->dmgDice;   
        ai.dmgBonus = attackItem->toDmg;  
      }
    }
  } else { // A spell-based attack.
    const SpellDesc& desc = Spell::spell(spell); 
    ai.attackDice = Dice(desc.dice.num, desc.dice.side);
  }

  {
    std::stringstream inf;
    inf << "attack roll " << ai.attackDice.n << "d" << ai.attackDice.x << ": ";
    // We save the above info for later, because we only generate attack-log-info at a later point
    // (and I want to list the info in a sensible order.)
    ai.dmgRoll = ai.attackDice.roll(inf); 
    ai.dmgRollInfo = inf.str();
  }

  if (school == SC_Phys) {
    ai.dmgMod = stats.statMod("str"); // You get your strength bonus added to dmg. IF it's physical.
  }
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


