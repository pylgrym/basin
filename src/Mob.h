#pragma once

#include <list>

#include "util/debstr.h"
#include "cellmap/cellmap.h"

#include "numutil/myrnd.h"

#include "Creature.h"

#include "Stats.h"

#include "Spell.h" // for attack schools.

#include "Dungeons.h"

struct AttackInf { // consider: combat-system should be isolated? 

  // HIT-OR-MISS info:
  int hitRoll; // the d20 hit roll.
  int wpHitBonus; // any bonus to our hit roll, e.g. plus-hit on weapon.
  int finalToHit; // player toHit plus weapon toHit.
  int advAC;
  int hitThres; // The limit we need to stay within.
  bool bHit; // whether we hit through adversarys armour-class or not.

  // DAMAGE INFO, in case of hit:
  Dice attackDice; // the weapon damage roll. (dmgDice?)

  int dmgRoll; // 'attackDice roll'.
  std::string dmgRollInfo;

  int dmgBonus; // dmg bonus from weapon.
  int dmgMod; // adjusters to dmg, e.g. %plus-dmg on weapon. and% str mod.
  int dmg; // final damage size we end up hitting with.
  int dmgTaken; // The actual amount of damage, adversary takes - adjusted for resistances/vulnerabilities.
  AttackInf() { hitRoll = 0; wpHitBonus = 0; finalToHit = 0; advAC = 0; hitThres = 0; bHit = false; dmgRoll = 0; dmgBonus = 0;  dmgMod = 0; dmg = 0; dmgTaken = 0; }

  double calcHitChance() const;

  void repHitChance(std::ostream& os); // the general probability.

  void repToHitCheck(std::ostream& os, Stats& stats) { // the current concrete instance.
    os << 'r' << hitRoll << "<=" << hitThres << " (th" << finalToHit << "-ac" << advAC << ")++" << wpHitBonus;
    os << " "; // Where does the horrible toHit come from..
    stats.repToHit(os);
  }
};


enum MoodEnum {
  M_Sleeping, // standing still, passively - may wake up if disturbed by user.
  M_Wandering, // bumbling around.
  M_Angry, // wants to attack player.
  M_Afraid, // fleeing from player, to survive and to trouble him (train him into other monsters.)
  M_Driven, // new clever logic.
  M_MaxMoods
};

class Mob {
public:
  Mob(int mlevel, bool bIsPlayer_); 
  ~Mob(void);

  bool persist(class Persist& p) {
    p.transfer(pos.x, "posx"); 
    p.transfer(pos.y, "posy"); 

    int intMobType = m_mobType;
    p.transfer(intMobType, "mobType"); 
    if (!p.bOut) { m_mobType = (CreatureEnum)intMobType;  }
      
    stats.persist(p);

    p.transfer(speed, "speed"); 

    int intMood = mood;
    p.transfer(intMood, "mood"); 
    if (!p.bOut) { mood = (MoodEnum) intMood;  }

    int intSchool = defSchool;
    p.transfer(intSchool, "defSchool"); 
    if (!p.bOut) { defSchool = (AttackSchool) intSchool; }

    int intMobSpell = mobSpell;
    p.transfer(intMobSpell, "mobSpell"); 
    if (!p.bOut) { mobSpell = (SpellEnum) intMobSpell; }

    p.transfer(mobDummyWeapon.n, "mobWeapon_n");
    p.transfer(mobDummyWeapon.x, "mobWeapon_x");

    return true;
  }

  CreatureEnum m_mobType; // better access it through ctype.
  CPoint pos;
  Stats stats;
  double speed;
  MoodEnum mood;
  Dice mobDummyWeapon;
  AttackSchool defSchool;
  virtual AttackSchool school() const { return defSchool;  }

  // IDEA: mob should be coloured with mobspell color.
  SpellEnum mobSpell; // IDEA: mob should have a high chance of dropping an item of this school.

  virtual bool isPlayer() const { return ctype() == CR_Player;  }

  bool calcAttack(std::ostream& os, AttackInf& ai, Mob& adv, Obj* attackItem, AttackSchool school, SpellEnum spell, bool overrideHit); // int& dmg);
  bool hitTest(class Mob& adv, struct AttackInf& ai); // int& roll, int hitBonus);
  int takeDamage(int dmg, AttackSchool damageType); // returns damage-taken (adjusted for resistancs/vulnerabilities)

  void recoverHP() {  // Delegate to stats class:
    stats.recoverHP();
  }
  void recoverMana() {  // Delegate to stats class:
    stats.recoverMana();
  }

  // Delegate to stats class:
  //void useMana(int manaCost) { stats.useMana(manaCost); }

  virtual int digStrength() { return 3;  }

  void makeAngry();

  //bool noticeAdv(Mob* adv);
  bool noticePlayer(double& chance); // Adv(Mob* adv);

  static double noticePlayerProb(CPoint coords, int mobAlert);


  Dice mobWeaponDice() { return mobDummyWeapon;  }

  bool nearPlayer() const;
  CPoint playerDir() const; // Norm-1 pointing direction.
  CPoint playerDelta() const; // Full pointing direction.
  bool lowHealth() const;

  static CPoint normDir(CPoint delta);

  void moveM(CPoint newpos); // 'does all', 'move mob on map'.

  bool playerOnStar() const; // If player is on a '8-star direction', we can use spells against him.
  bool canSee(CPoint b, bool onlyEnvir);
  bool canSeePlayer();
  bool canPass(CPoint newpos);

  bool canChase(CPoint target);
  bool canFlee(CPoint target);

  bool mobCasts(CPoint dir);
  bool mobMeleesPlayer(CPoint dir);
  bool chasePlayer(CPoint dir);

  COLORREF color;
  virtual CreatureEnum ctype() const { return m_mobType; } // = 0;
  const MobDef&  mobDef();

  virtual double act() = 0; // returns time that action requires (0 means keep doing actions/keep initiative.)
  virtual void passTime() {} // Currently only activated for player mob.

  virtual bool wear(Obj* obj, std::ostream& err); // Obj will go to/from bag.

  virtual std::string a_mob()  const = 0;
  virtual std::string the_mob()  const { return pronoun(); } // just an alias..
  virtual std::string pronoun() const = 0; // "You"/"The orc".
  virtual std::string verbS() const = 0; // "he hitS".

  // JG, NB! 'isDead' doesn't really have to be a member/virtual,
  // it's more like a general 'property statement' (ie "is hp >= 0"),
  // however, having it as member, allows us to easily determine that we can ask this property of a mob.
  //  NB - Being dead, apart from being a winning/losing condition,
  // is mainly the property that determines whether you still have a place
  // in the event queue! hmm, could be 'is alive' too..
  bool isDead() const { return stats.hp < 0; } // Even 0 is alive.. // not virtual, so far.
  void invalidateGfx(CPoint pos, CPoint oldpos, bool force);
  void invalidateGfx(); // A second helper.

  bool canCarry(class Obj*) const { return true; } //  oneIn(2) == 1;

  void lightWalls() { // CPoint pos) {
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        CPoint p(pos.x + dx, pos.y + dy);
        CL->map[p].markWalls(p); 
      }
    }
  }

  void lightArea(CPoint pos, int radius) {
    for (int dx = -radius; dx <= radius; ++dx) {
      for (int dy = -radius; dy <= radius; ++dy) {
        CPoint p(pos.x + dx, pos.y + dy);
        if (CL->map.legalPos(p)) {
          CL->map[p].lightCells(p);//  Walls();
        }
      }
    }
  }

};


















class MonsterMob : public Mob {
public:
  MonsterMob(int mlevel) :Mob(mlevel, false) {}

  virtual double act(); // returns time that action requires (0 means keep doing actions/keep initiative.)

  virtual double actSleep();
  virtual double actWander();
  virtual double actAngry();
  virtual double actDriven();
  virtual double actFlee();

  virtual double actGeneric();

  virtual std::string a_mob() const;
  virtual std::string pronoun() const; //(FIXME, maybe should be called 'the_mob' instead of 'pronoun'. { return "you";  } // "You"/"The orc".
  virtual std::string verbS() const { return "s";  } // "he hitS".



  bool hurt(); //  { return false; }
  bool can_flee(); //  { return false; }
  bool flee_prob(); //  { return false; }
  bool flee(); //  { return false; }

  bool can_attack(); //  { return false; }
  bool hurt_attack_prob(); //  { return false; }
  bool attack(); //  { return false; }

  bool too_close(); //  { return false; }
  bool can_incr(); //  { return false; }
  bool incr_prob(); //  { return false; }
  bool incr_dist(); //  { return false; }

  bool too_far(); //  { return false; }
  bool can_decr(); //  { return false; }
  bool decr_prob(); //  { return false; }
  bool decr_dist(); //  { return false; }

  bool melee_range(); //  { return false; }
  bool melee_prob(); //  { return false; }
  bool attack_melee(); //  { return false; }

  bool can_ranged(); //  { return false; }
  bool ranged_prob(); //  { return false; }
  bool attack_ranged(); //  { return false; }

  bool stay(); //  { return false; }

};

