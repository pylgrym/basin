#pragma once

#include <list>
#include <queue>
#include <functional> // for greater.

#include "util/debstr.h"
#include "cellmap/cellmap.h"

#include "numutil/myrnd.h"

#include "Creature.h"

#include "Stats.h"

#include "Spell.h" // for attack schools.

struct AttackInf {
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
  int dmgBonus; // dmg bonus from weapon.
  int dmgMod; // adjusters to dmg, e.g. %plus-dmg on weapon. and% str mod.
  int dmg; // final damage size we end up hitting with.
  int dmgTaken; // The actual amount of damage, adversary takes - adjusted for resistances/vulnerabilities.
  AttackInf() { hitRoll = 0; wpHitBonus = 0; finalToHit = 0; advAC = 0; hitThres = 0; bHit = false; dmgRoll = 0; dmgBonus = 0;  dmgMod = 0; dmg = 0; dmgTaken = 0; }

  double calcHitChance() const;
  void repHitChance(std::ostream& os);

  void rep(std::ostream& os, Stats& stats) {
    os << 'r' << hitRoll << "<=" << hitThres << " (th" << finalToHit << "-ac" << advAC << ")+" << wpHitBonus;
    os << " "; // Where does the horrible toHit come from..
    stats.repToHit(os);
  }
};


enum MoodEnum {
  M_Sleeping, // standing still, passively - may wake up if disturbed by user.
  M_Wandering, // bumbling around.
  M_Angry, // wants to attack player.
  M_Afraid, // fleeing from player, to survive and to trouble him (train him into other monsters.)
  M_MaxMoods
};

class Mob {
public:
  Mob(void);
  ~Mob(void);

  CreatureEnum m_mobType; // better access it through ctype.
  CPoint pos;
  Stats stats;
  double speed;
  MoodEnum mood;
  Dice mobDummyWeapon;
  AttackSchool defSchool;
  virtual AttackSchool school() const { return defSchool;  }

  virtual bool isPlayer() const { return ctype() == CR_Player;  }

  bool calcAttack(Obj* attackItem, class Mob& adv, struct AttackInf& ai, AttackSchool school, std::ostream& os); // int& dmg);
  bool hitTest(class Mob& adv, struct AttackInf& ai); // int& roll, int hitBonus);
  int takeDamage(int dmg, AttackSchool damageType); // returns damage-taken (adjusted for resistancs/vulnerabilities)

  virtual int digStrength() { return 3;  }

  void makeAngry();

  Dice mobWeaponDice() { return mobDummyWeapon;  }

  bool nearPlayer() const;
  CPoint playerDir() const;
  bool lowHealth() const;

  COLORREF color;
  virtual CreatureEnum ctype() const { return m_mobType; } // = 0;

  virtual double act() = 0; // returns time that action requires (0 means keep doing actions/keep initiative.)
  virtual void passTime() {} // Currently only activated for player mob.

  virtual bool wear(Obj* obj, std::ostream& err); // Obj will go to/from bag.

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

  void lightWalls(CPoint pos) {
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        CPoint p(pos.x + dx, pos.y + dy);
        Map::map[p].markWalls(); 
      }
    }
  }

  void lightArea(CPoint pos, int radius) {
    for (int dx = -radius; dx <= radius; ++dx) {
      for (int dy = -radius; dy <= radius; ++dy) {
        CPoint p(pos.x + dx, pos.y + dy);
        if (Map::map.legalPos(p)) {
          Map::map[p].lightCells();//  Walls();
        }
      }
    }
  }

};



struct MobReady {
  MobReady(double when_, Mob* mob_) :when(when_), mob(mob_) {}

  double when;
  Mob* mob;

  bool operator < (const MobReady& rhs) const { return when < rhs.when; }
  bool operator >(const MobReady& rhs) const { return when > rhs.when; }
};





template<class T, class Compare>
class PQV : public std::vector<T> { // Needed because std priority-queue is immutable.
  Compare comp;
public:
  PQV(Compare cmp = Compare()) : comp(cmp) {
    makeHeap();
  }
  void makeHeap() { std::make_heap(this->begin(), this->end(), comp); }

  const T& top() { return this->front(); }
  void push(const T& x) {
    this->push_back(x);
    //std::vector<T>::iterator a = this->begin();
    //std::vector<T>::iterator b = this->end();
    std::push_heap(this->begin(), this->end(), comp);
  }
  void pop() {
    std::pop_heap(this->begin(), this->end(), comp);
    this->pop_back();
  }
};

typedef std::deque<MobReady> ReadyDeque;
typedef std::priority_queue<MobReady, ReadyDeque, std::greater<MobReady> > ReadyQueueB;
//typedef std::priority_queue<MobReady,ReadyDeque, std::less<MobReady> > ReadyQueue;
typedef PQV<MobReady, std::greater<MobReady> > ReadyQueue;




class MobQueue {
public:
  //int meat1;
  ReadyQueue queue;
  //int meat2;
  // FIXME, maybe should be a set, not a vector:
  std::vector<Mob*> globalMobs; // an 'owner' of all mobs, no priority.
  double globalClock;

  MobQueue(); // :globalClock(0), meat1(0xDEADBEEF), meat2(0xDEADBEEF) { }

  void queueMob(Mob* mob, double delay) {
    double when = globalClock + delay;
    // MobQueue* that = this;
    queue.push(MobReady(when, mob)); // push_heap.
    globalMobs.push_back(mob);
    /* push, top, pop. push_heap, make_heap, pop_heap. (sort_heap?)
    */
  }

  void deleteMob(Mob* toDelete);
  bool dispatchFirst();

  static MobQueue mobs;
};


class PlayerMob : public Mob {
public:
  PlayerMob();
  ~PlayerMob();
  virtual double act(); // returns time that action requires (0 means keep doing actions/keep initiative.)
  virtual void passTime();
  void updateLight();
  Obj* findLight();

  virtual CreatureEnum ctype() const { return CR_Player; } // Consider not having this.. (instead just relying on base ctype)

  virtual std::string pronoun() const { return "you";  } // "You"/"The orc".
  virtual std::string verbS() const { return "";  } // "you HIT".

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
};




class MonsterMob : public Mob {
public:
  virtual double act(); // returns time that action requires (0 means keep doing actions/keep initiative.)

  virtual double actSleep();
  virtual double actWander();
  virtual double actAngry();
  virtual double actFlee();


  // virtual CreatureEnum ctype() const { return CR_Kobold; }
  virtual std::string pronoun() const; // { return "you";  } // "You"/"The orc".
  virtual std::string verbS() const { return "s";  } // "he hitS".
};

