#pragma once

#include "Cmd.h"

#include "Mob.h"
#include <assert.h>

#include "Bag.h"

#include "theUI.h"

#include "Cuss.h"

#include "Creature.h"
#include "LogEvents.h"

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

    if (Map::map[newpos].blocked()) {
      err << "That way is blocked."; // The way ahead is blocked.
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
    hittee = Map::map[tgt].creature.m; 
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
    bool canDig = (Map::map[tgt].envir.type == EN_Wall);
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
    o = Map::map[tgt].item.o;
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



class TakeCmd : public Cmd {
public:
  CPoint tgt;
  Mob& mob;
  Obj* o;

  TakeCmd(Mob& mob_):mob(mob_) {
    tgt = mob.pos;
    o = Map::map[tgt].item.o;
  }

  virtual bool silent() const { return !mob.isPlayer(); } // .ctype() != CR_Player;  }

  virtual bool legal(std::ostream& err) { 
    bool empty = Map::map[tgt].item.empty(); 
    // debstr() << "empty?" << empty << "\n";

    if (empty) { err << "There is nothing to pick up.";  return false; }
    // debstr() << "can carry?" << mob.canCarry(o) << "\n";

    if (!mob.canCarry(o)) { err << "It's too heavy to lift."; return false; }
    return true;  
  }


  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    ObjSlot& item = Map::map[tgt].item;
    // (No need to invalidate, as we are standing on it)
    mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on Map::map/Cell! (maybe)


    if (o->otype() == OB_Gold || o->otype() == OB_Gems || o->otype() == OB_Emeralds || o->otype() == OB_Amethysts ) { // Gold is special - it's consumed on pickup, and added to gold balance:
      mob.stats.gold += o->itemUnits;
      std::string idesc = o->indef_item();
      err << "You pick up " << o->itemUnits << " " << idesc; // gold pieces.";
      item.setObj(NULL);
      delete o; 
      return true; 
    } // special-case gold.

    // Everything else non-gold, goes in bag:

    if (!Bag::bag.add(o, err)) { return false; } // err << "Couldn't fit item in bag.";  

    char itemIx = Bag::bag.letterIx(o);
    // std::string anItem = o->an_item();
    std::string theItem = o->the_item();
    err << "You pick up (" << itemIx << ") " << theItem;
    item.setObj(NULL);
    return true; 
  }
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
    bool empty = Map::map[tgt].item.empty(); 
    debstr() << "empty?" << empty << "\n";

    if (!empty) { 
      err << "No space here."; // There is no room to drop anything here."; // "Can't drop here."; //  
      return false; 
    }
    return true;  
  }


  virtual bool Do(std::ostream& err) {  
    debstr() << "drop-cmd do begin..\n";
    if (!Cmd::Do(err)) { 
      return false; 
    }

    debstr() << "doing drop item-command.\n";
    Obj* obj = Bag::bag.pick("  Drop what?");
    if (obj == NULL) { return false;  }

    debstr deb;
    if (!Bag::bag.remove(obj, deb)) { deb << "\n";  return false; } 

    ObjSlot& item = Map::map[tgt].item;
    item.setObj(obj);
    // (No need to invalidate, as we are standing on it)
    mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on Map::map/Cell! (maybe)

    std::string anItem = obj->an_item();
    err << "You drop " << anItem; 
    return true; 
  }
};




class InvCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing bag Inventory command.\n";

    Cuss::clear(false);
    Cuss::prtL("  Your bag contains:"); 

    Bag::bag.showInv();

    TheUI::promptForAnyKey(); 

    Cuss::clear(true);
    return true; 
  }
};



class UnequipCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing unequip item-command.\n";

    /*
    Cuss::clear(false);
    Cuss::prtL("  You are wearing:"); 
    Equ::worn.showWorn();
    TheUI::promptForAnyKey(); 
    Cuss::clear(true);
    */

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

  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "doing use item-command.\n";
    Obj* obj = Bag::bag.pick("  Use what?");
    if (obj == NULL) { return false;  }

    if (obj->wearable()) {
      return mob.wear(obj, err);
    }

    if (!obj->use(mob, err)) {
      return false;
    }
    if (obj->charges == 0 && obj->consumed) {
      err << "You've used up the item.";
      Bag::bag.remove(obj, err);
      delete obj;
    }
    return true; 
  }
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
    Obj* obj = Bag::bag.pick("  Examine what?");
    if (obj == NULL) { return false;  }

    logstr log; 
    log << "Charges:" << obj->charges;
    log << ", perish:" << obj->consumed;
    log << ", units:" << obj->itemUnits;
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

    if (mob.stats.hp < mob.stats.maxHP) {
      bool chance = oneIn(6);
      if (chance) { 
        mob.recoverHP(); // ++mob.stats.hp;
        mob.invalidateGfx();
        logstr log;
        log << "You regain some health!";
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

  ZapCmd(Obj* hitItem_, Mob& mob_, SpellEnum effect_, AttackSchool school_)
  :mob(mob_), effect(effect_),school(school_), zapHitItem(hitItem_) {
    tgt = mob.pos;
  }

  virtual bool silent() const { return !mob.isPlayer(); } 
  virtual bool legal(std::ostream& err) {  return true; }

  virtual bool Do(std::ostream& err);
};


class StatCmd : public Cmd {
public:
  virtual bool Do(std::ostream& err) {  
    if (!Cmd::Do(err)) { return false; }

    debstr() << "show stats command.\n";

    Cuss::clear(false);
    Cuss::prtL("  Your stats:"); 

    PlayerMob::ply->stats.showStats(); 
    TheUI::promptForAnyKey(); 

    Cuss::clear(true);
    return true; 
  }
};
