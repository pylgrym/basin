#pragma once

#include <list>
#include <queue>
#include <functional> // for greater.

#include "util/debstr.h"
#include "cellmap/cellmap.h"

#include "numutil/myrnd.h"

#include "Creature.h"

#include "Stats.h"

enum AttackSchool {
  // Ideas for different kinds of attack, for weakness/strength against.
  SC_Phys,
  SC_Fire,
  SC_Frost,
  SC_Water,
  SC_Lightning,
  SC_Air,
  SC_Earth,
  SC_Acid,
  SC_Shadow,
  SC_Magic,
  SC_Nature,
  SC_Undead,
  SC_Holy
};


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

  void rep(std::ostream& os, Stats& stats) {
    os << 'r' << hitRoll << "<=" << hitThres << " (th" << finalToHit << "-ac" << advAC << ")+" << wpHitBonus;
    os << " "; // Where does the horrible toHit come from..
    stats.repToHit(os);
  }
};


class Mob {
public:
  Mob(void);
  ~Mob(void);

  CPoint pos;
  // int hp;
  // int maxhp;
  Stats stats;
  Dice mobDummyWeapon;

  bool calcAttack(class Mob& adv, struct AttackInf& ai); // int& dmg);
  bool hitTest(class Mob& adv, struct AttackInf& ai); // int& roll, int hitBonus);
  int takeDamage(int dmg, AttackSchool damageType); // returns damage-taken (adjusted for resistancs/vulnerabilities)

  Dice mobWeaponDice() { return mobDummyWeapon;  }

  COLORREF color;
  virtual CreatureEnum ctype() const = 0;

  virtual double act() = 0; // returns time that action requires (0 means keep doing actions/keep initiative.)

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
  ReadyQueue queue;
  // FIXME, maybe should be a set, not a vector:
  std::vector<Mob*> globalMobs; // an 'owner' of all mobs, no priority.
  double globalClock;

  MobQueue() :globalClock(0) { }

  void queueMob(Mob* mob, double delay) {
    double when = globalClock + delay;
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
  virtual CreatureEnum ctype() const { return CR_Player; }
  virtual std::string pronoun() const { return "you";  } // "You"/"The orc".
  virtual std::string verbS() const { return "";  } // "you HIT".

  static PlayerMob* ply;
  static int distPly(CPoint p);
};

class MonsterMob : public Mob {
public:
  virtual double act(); // returns time that action requires (0 means keep doing actions/keep initiative.)
  virtual CreatureEnum ctype() const { return CR_Kobold; }
  virtual std::string pronoun() const; // { return "you";  } // "You"/"The orc".
  virtual std::string verbS() const { return "s";  } // "he hitS".
};

