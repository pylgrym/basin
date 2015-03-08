#pragma once

#include "Cmd.h"

#include "Mob.h"
#include <assert.h>

#include "Bag.h"

#include "theUI.h"

#include "Cuss.h"

#include "Creature.h"
#include "LogEvents.h"

#include <assert.h>

class WalkCmd : public Cmd {
public:
  WalkCmd(class Mob& mob_, int dx, int dy, bool force):old(mob_.pos), mob(mob_) {
    // JG: I'm not sure about force - the intent is, that with 'force', you can make sure Walk will 'do what you tell it to do', even if player is challenged/confused.
    if (mob.stats.isConfused() && oneIn(2) && !force) {
      logstr log; log << "You stumble randomly, still confused.";
      dx = rndC(-1, 1); dy = rndC(-1, 1);
    }
    newpos = old; newpos.x += dx; newpos.y += dy;
  }

  virtual bool silent() const { return !mob.isPlayer(); } 

  virtual bool legal(std::ostream& err) { 
    if (newpos == old) { err << "Not a move-direction.";  return false; }

    if (CL->map[newpos].blocked()) {
      Envir& env = CL->map[newpos].envir;
      err << "That way is blocked by " << env.typestr() <<  "."; // The way ahead is blocked.
      return false;
    }
    return true;
  }

  CPoint old,newpos;
  Mob& mob;

  virtual bool Do(std::ostream& err);
};



class HitCmd : public Cmd {
public:
  HitCmd(Obj* hitItem_, Mob& mob_, int dx, int dy, AttackSchool school_):mob(mob_), hittee(NULL), school(school_), hitItem(hitItem_) {
    tgt = mob.pos; tgt.x += dx; tgt.y += dy;
    hittee = CL->map[tgt].creature.m; 
  }

  virtual bool silent() const { return !mob.isPlayer(); }  

  virtual bool legal(std::ostream& err) { 
    if (hittee == NULL) { 
      if (!silent()) { err << "No opponent to hit."; }
      return false; 
    }
    if (hittee == &mob) { err << "Don't hit ourselves..";  return false;  }
    return true; 
  }

  CPoint tgt;
  Mob& mob;
  Mob* hittee;
  AttackSchool school;
  Obj* hitItem;

  virtual bool Do(std::ostream& err);

};



class DigCmd : public Cmd {
public:
  DigCmd(Mob& mob_, int dx, int dy):mob(mob_) {
    tgt = mob.pos; tgt.x += dx; tgt.y += dy;
  }

  virtual bool silent() const { return !mob.isPlayer(); } // .ctype() != CR_Player;  }

  virtual bool legal(std::ostream& err) { 
    if (tgt == mob.pos) { err << "Not a dig-direction.\n";  return false; }
    bool canDig = (CL->map[tgt].envir.type == EN_Wall);
    if (!canDig) { err << "You can't dig through that.";  }
    return canDig;  
  }

  CPoint tgt;
  Mob& mob;

  virtual bool Do(std::ostream& err);
};





class LookCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;
  Obj* o;

  LookCmd(Mob& mob_):mob(mob_) {
    tgt = mob.pos;
    o = CL->map[tgt].item.o;
  }

  virtual bool silent() const { return !mob.isPlayer(); } // .ctype() != CR_Player;  }
  virtual bool legal(std::ostream& err) {  return true; }

  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    err << "You see ";
    if (o == NULL) { 
      err << "nothing here.";  
    } else {
      std::string anItem = o->an_item();
      err << anItem << ".";
    }
    return true; 
  }
};


class StairCmd : public Cmd {
public:
  Mob& mob;
  EnvirEnum etype;
  StairCmd(Mob& mob_) :mob(mob_) { etype = CL->map[mob.pos].envir.type; }
  virtual bool silent() const { return !mob.isPlayer(); } 

  virtual bool legal(std::ostream& err) {
    if (etype != EN_StairDown && etype != EN_StairUp) { err << "There are no stairs here.\n";  return false; }
    return true;
  }

  virtual bool Do(std::ostream& err);
};



class TakeCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;
  Obj* o;

  TakeCmd(Mob& mob_):mob(mob_) {
    tgt = mob.pos;
    o = CL->map[tgt].item.o;
  }

  virtual bool silent() const { return !mob.isPlayer(); } // .ctype() != CR_Player;  }

  virtual bool legal(std::ostream& err) { 
    bool empty = CL->map[tgt].item.empty(); 
    // debstr() << "empty?" << empty << "\n";

    if (empty) { err << "There is nothing to pick up.";  return false; }
    // debstr() << "can carry?" << mob.canCarry(o) << "\n";

    if (!mob.canCarry(o)) { err << "It's too heavy to lift."; return false; }
    return true;  
  }

  virtual bool Do(std::ostream& err);
};




class DropCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;

  DropCmd(Mob& mob_):mob(mob_) {
    tgt = mob.pos;
  }

  virtual bool silent() const { return !mob.isPlayer(); } // .ctype() != CR_Player;  }

  virtual bool legal(std::ostream& err) { 
    bool empty = CL->map[tgt].item.empty(); 
    debstr() << "empty?" << empty << "\n";

    if (!empty) { 
      err << "No space here."; // There is no room to drop anything here."; // "Can't drop here."; //  
      return false; 
    }
    return true;  
  }


  virtual bool Do(std::ostream& err);
};




class InvCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing bag Inventory command.\n";

    Cuss::clear(false);
    Cuss::prtL("  Your bag contains:"); 

    Bag::bag.showBagInv(false);

    TheUI::promptForAnyKey(__FILE__, __LINE__, "inv-pause");

    Cuss::clear(true);
    return true; 
  }
};



class SpellInvCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};


class UnequipCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing unequip item-command.\n";

    Obj* obj = Equ::worn.pick("  Unequip what?");
    if (obj == NULL) { return false;  }
    if (!Bag::bag.canAdd()) { err << "Your bag is too full."; return false;  }
    if (!Equ::worn.unequipItem(obj, err)) {
      return false;
    }
    Bag::bag.add(obj, err);
    return true; 
  }
};



/*
class WearCmd : public Cmd {
// WEAR is not necessary, since it's just 'USE' - if you 'use' something you can wear..
public:
  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing wear item-command.\n";
    Obj* obj = Bag::bag.pick("  Wear what?");
    if (obj == NULL) { return false;  }
    return true; 
  }
};
*/




class UseCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;

  UseCmd(Mob& mob_):mob(mob_) {
    tgt = mob.pos;
  }

  virtual bool Do(std::ostream& err);
};




class ExamineCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;

  ExamineCmd(Mob& mob_):mob(mob_) {
    tgt = mob.pos;
  }

  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing examine item-command.\n";
    Obj* obj = Bag::bag.pickBag("  Examine what?", false);
    if (obj == NULL) { return false;  }

    logstr log; 
    log << "Charges:" << obj->charges;
    log << ", perish:" << obj->consumed;
    log << ", units:" << obj->itemUnits;

    // FIX to show better info!
    return true; 
  }
};



class ShowEventsCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};




class WaitCmd : public Cmd {
public:
  Mob& mob;

  WaitCmd(Mob& mob_):mob(mob_) {
  }

  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    {
      logstr log;
      log << "You wait.";
    }

    bool chance = oneIn(4);
    if (chance) { 
      if (mob.stats.hp < mob.stats.maxHP) { // resting
        mob.invalidateGfx();
        mob.recoverHP();
        logstr log;
        log << "You regain some health!";
      }
      if (mob.stats.mana < mob.stats.maxMana) { // resting
        mob.recoverMana();
      }
    }
    return true; 
  }
};




class ZapCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;
  SpellEnum effect;
  AttackSchool school;
  Obj* zapHitItem;
  bool consumeMana; // HACK - can we control this more elegantly? also, mana cost on spells.
  CPoint mobZapDir; // Only used when monsters cast spells.

  ZapCmd(Obj* hitItem_, Mob& mob_, SpellEnum effect_, AttackSchool school_)
  :mob(mob_), effect(effect_),school(school_), zapHitItem(hitItem_), consumeMana(false) {
    tgt = mob.pos;
  }

  virtual bool silent() const { return !mob.isPlayer(); } 
  virtual bool legal(std::ostream& err);

  virtual bool Do(std::ostream& err);
};


class StatCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};


class SaveCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};

class LoadCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};


class ShopCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};

class BuyCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};

class SellCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err);
};



class DexModCmd : public Cmd {
public:
  int delta;
  DexModCmd(int delta_) :delta(delta_){}
  virtual bool Do(std::ostream& err);
};


class FillLampCmd : public Cmd {
public:
  Obj* oil;
  FillLampCmd(Obj* oil_) :oil(oil_){}
  virtual bool Do(std::ostream& err);
};



class CastCmd : public Cmd {
public:
  Mob& mob;
  CastCmd(Mob& mob_):mob(mob_) {}
  virtual bool Do(std::ostream& err);
};


class DoorToggleCmd : public Cmd {
public:
  Mob& mob;
  DoorToggleCmd(Mob& mob_):mob(mob_) {}
  virtual bool Do(std::ostream& err);
};

class DoorBashCmd : public Cmd {
public:
  Mob& mob;
  DoorBashCmd(Mob& mob_):mob(mob_) {}
  virtual bool Do(std::ostream& err);
};
