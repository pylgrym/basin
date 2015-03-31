#pragma once

#include <map>
#include <string>
#include <vector>
#include "numutil/myrnd.h"

typedef std::string ModSource;
typedef int ModDelta;
typedef std::map<ModSource, ModDelta> ModColl;


class Stat {
public:
  Stat(const std::string& name_) :name(name_) { base = 0; onusBonus = 0; roll(); }
  // Stat():name("nostat?") { v = 0; roll();  }

  int v() const { return base + onusBonus;  }
  int base; // value.

  /* onusBonus: temporary penalties,bonuses, adjustments to stat (must be made in a different way, so we can keep track of different contributions - 
  e.g. a push/pop stack of 'tags', added to a map, e.g. std::map< std::string(tag), deltaInt >
  */
  int onusBonus;
  ModColl mods; // better approach than onusBonus - named/tagged modifiers, we can find/add/remove.
  int sumMods() // const 
  { 
    int total = 0;
    ModColl::iterator i;
    for (i = mods.begin(); i != mods.end(); ++i) {
      ModDelta& md = i->second;
      total += md;
    }
    return total;
  }
   
  std::string name;
  void roll();
  int mdf() const; // Beware, I have two different mechanisms active for this..
  bool rollCheck(bool guifeedback) const; // if true, may trigger "Wow!/Argh!" messages.

  void makeAvg() { base = 11; }

  bool persist(class Persist& p);
};

/* I don't like this interface,
I think I'll roll a mix of named fields and enums. */
/*
enum StatsEnum { 
  STREN,
  INTEL,
  DEXTE,
  STAMI,
  WISDO,
  CHARI,
};
*/

class TmpState {
public:
  TmpState() :dur(0) {}
  int dur; // remaining duration for the temp effect.

  /* note, this class itself is  not useful for the flavor texts
   - you'd get stuff like 'you feel less blind'. :-(
  */
  void updateEffect(int input) { // NB - both works for additon and for clearing (a questionable design.)
    if (input == 0) {
      clearEffect();
    } else {
      addDur(input);
    }
  }

  void clearEffect() {
    dur = 0;
  }

  void addDur(int addition) {
    dur += addition;
  }

  bool tickEffect(); // countdown for duration. returns false if not active.
};


class Stats // Actually, ability stats..
{
public:
  Stat Str, Int, Dex, Con, Wis, Chr;
  //std::map< std::string, Stat > s;
  // std::map< std::string, Stat* > s;

  Stat& stat(const char* name);

  // void addStat(const std::string& name);
  Stats(int level_, bool bPlayer_);
  ~Stats();

  void makeAvg() {
    Str.makeAvg(); Int.makeAvg(); Dex.makeAvg(); 
    Con.makeAvg(); Wis.makeAvg(); Chr.makeAvg();
  }

  bool isPlayer; // important for some calcs, because of singleton-silliness (no player stats for mobs..)
  int theLevel;
  void setLevel(int level_);
  int level() const { return theLevel;  }
  int lvlAdj(int in) const;


  void initStats();
  void calcStats();

  enum StatEnums { HitDie = 5};

  //int baseHP; 
  // std::vector<int> hp_rolls; 
  int hp;
  int maxHP;

  int xp;
  int xpTotal;
  int xpToLevel;

  int mana;
  int maxMana;
  std::vector<int> manaRolls; // These are fixed from start - they must be persisted!

  int ac;
  int toHit;
  int baseMobAC; // a varying dice roll for mob ac, a 'delta' (to make mobs not always equal)
  int wornAC_input, wornAC_output;

  // Temp/fluctuating state: Might deserve its own sub-struct; 
  // OTOH, actually all stats might need to follow..
  int hunger;
  // FIXME - these should go into a dynamic map.., maybe? or maybe I'm confusing it with tmp-modifiers.
  TmpState s_confused; // counter > 0 if we are confused, 0 if not confused. 
  TmpState s_afraid; // can walk, but can't hit
  TmpState s_blinded; // can't see/aim/read spells, but can operate objects and walk
  TmpState s_rooted; // You can hit, but you can't walk.
  TmpState s_poisoned; // You lose health quickly.
  // Sleepy-drowsy?
  // On fire? freezing?
  //int paralyzed; //
  // nauseous, vomiting, starving, ?

  int gold; // Possibly this should go in equipment/inventory instead..?

  bool isConfused() const { return (s_confused.dur > 0);  }
  bool isAfraid() const { return (s_afraid.dur > 0);  }
  bool isBlind() const { return (s_blinded.dur > 0);  }
  bool isRooted() const { return (s_rooted.dur > 0);  }
  bool isPoisoned() const { return (s_poisoned.dur > 0);  }

  int calcMaxHP();
  int calcMaxMana();

  int calcTotalAC();
  int calcBaseAC();
  int mobAC(); // the AC a mob gets to have, since he doesn't have 'worn items'.

  int itemACReduc(int wornAC); // Player will earn diminished AC from items.

  int calcToHit(std::ostream& os);

  void repToHit(std::ostream& os) {
    // os << "xyz";
    calcToHit(os);
  }

  // DERIVED STATS:
  int stealth() const; // based on dex mod + level.
  int alertness() const; // based on WIS mod + level.

  void healPct(int percent, class Mob*); // mob along, so we can make log messages.
  void healAbs(int val, Mob* mob);

  bool useMana(int manaCost);
  void manaPct(int percent, class Mob*); // mob along, so we can make log messages.

  void recoverHP() { // Randomized healing.
    int frac = (maxHP / 10)+1;
    int inst = rnd::Rnd(frac / 2, frac+ frac/2);
    int newHP = hp + inst;
    if (newHP > maxHP) { newHP = maxHP;  }
    hp = newHP;
  }

  void recoverMana() { // Randomized mana recov.
    int frac = (maxMana / 10)+1;
    int inst = rnd::Rnd(frac / 2, frac+ frac/2);
    if (inst < 1) { inst = 1; }
    int newMana = mana + inst;
    if (newMana > maxMana) { newMana = maxMana;  }
    mana = newMana;
  }

  static void passWorldTime(); // only for player.
  void passTime(Mob* who);
  void updateHunger(Mob* who);

  void tickConfusion(Mob* who);
  void tickFear(Mob* who);
  void tickBlinded(Mob* who); // fixme -should temp-unlight cells?
  void tickRooted(Mob* who);
  void tickPoisoned(Mob* who); // should reduce health.


  void showStats();

  void initXP();
  void gainKillXP(int mobLevel);
  void gainLevel();

  bool persist(class Persist& p);

  // static Stats stats;
  static int statModifyEffect[18 + 1];
  int statMod(const std::string& stat);
  static int mdf(int stat); // Alternative approach

  static int calcAvgACeffect();
};

