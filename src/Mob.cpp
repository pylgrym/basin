#include "stdafx.h"
#include "Mob.h"

#include "./theUI.h"

#include "cellmap/cellmap.h"

#include "numutil/myrnd.h"

#include <assert.h>

#include "Cmds.h"

#include "LogEvents.h"

#include "Equ.h"

#include <iomanip>

Mob::Mob() {  
  mobDummyWeapon = Dice(rnd(3), rnd(12)); // Wow that can hit hard..

  pos.x = rnd(1, Map::Width-1); 
  pos.y = rnd(2, Map::Height-1);
  color = RGB(rand()%255,rand()%255,rand()%255);
}


Mob::~Mob() {}



void Mob::invalidateGfx(CPoint pos, CPoint oldpos, bool force) {
  // JG: I'm not so sure about this method design, looks ugly to me. pos/oldpos is OK, but 'force' actually means 'another method' :-(.
  if (pos != oldpos || force) {
    TheUI::invalidateCell(oldpos); 
    TheUI::invalidateCell(pos); 
  }
}

void Mob::invalidateGfx() {
  TheUI::invalidateCell(pos); 
}













double PlayerMob::act() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  double actionDuration = 1.0; // seconds. // WalkCmd/Cmd's might set this instead.

  bool bActionDone = false;
  for (;!bActionDone;) {
	  // Prompt user for command, then move:
	  int nChar = TheUI::getNextKey(); 

    LogEvents::log.resetNotif();

    // Clear any left-over msg:
    Cuss::move(CPoint(0, 0));
    Cuss::prtL("");

    bool bShift = TheUI::shiftKey(); // Shift is running, but doesn't exist yet.
    bool bCtrl = TheUI::ctrlKey();

    // std::stringstream ss;
    logstr ss;

    switch (nChar) {
    case 'W': // W=wield/wear, same as a=activate/use.
    case 'A': if ( UseCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true; } break; // Use is.. A..?

    case 'D': if (DropCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true; } break; // D is DROP.
    case 'G': if (TakeCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true;  } break; // G is GET.
    case 'I': if (InvCmd().Do(ss))       { actionDuration = 0; bActionDone = true; } break; // I is INVentory (not B-bag.)
    case 'P': if (bCtrl) { if (ShowEventsCmd().Do(ss)) { actionDuration = 0; bActionDone = true; } break; } // ctrl-P is 'show event log'.
    case 'E': if (UnequipCmd().Do(ss))   { actionDuration = 1; bActionDone = true; } break; // T is 'take-off', as U=unequip is already used.

    case '.': 
    case VK_OEM_PERIOD: if (WaitCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true; } break; 

    case 'S': if (LookCmd(*this).Do(ss)) { actionDuration = 0; bActionDone = true; } break; 



    // Move/hit/dig: 
	  case VK_RIGHT: case 'L': case 'U': case 'N': 
    case VK_LEFT:  case 'H': case 'Y': case 'B':
	  case VK_DOWN:  case 'K': 
    case VK_UP:    case 'J': 
      { // scope for movement/hit/dig:
        int dx = 0, dy = 0;
        // determine movement:
        switch (nChar) { case VK_RIGHT:  case 'L': case 'U': case 'N': dx = 1;  }
        switch (nChar) { case VK_LEFT:   case 'H': case 'Y': case 'B': dx = -1; } 
        switch (nChar) { case VK_DOWN:   case 'J': case 'B': case 'N': dy = 1;  } 
        switch (nChar) { case VK_UP:     case 'K': case 'Y': case 'U': dy = -1; } 
        
        CPoint target(pos.x + dx, pos.y + dy);

        if (Map::map[target].creature.empty()) {
          if (bCtrl) { // CTRL means digging:
            if (DigCmd(*this, dx, dy).Do(ss)) { actionDuration = 1; bActionDone = true; }
          } else { // no control-key - it's a move.
            if (WalkCmd(*this, dx, dy).Do(ss)) { actionDuration = 1; bActionDone = true; }
          }
        } else { // target-field HAS a creature - then it's an attack (we aren't very social, are we..)
          if (HitCmd(*this, dx, dy).Do(ss)) { actionDuration = 1; bActionDone = true; }
        }
      } //end movement-scope-block.
      break; // end of keyboard switch.
    } // end switch keyboard.

  } // wait-for-successful-command loop.

	return actionDuration;
}





double MonsterMob::act() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  const double duration = 1.0; // actionDuration  // seconds.

	// stagger to a random location:
  int dx = rndC(-1, 1), dy = rndC(-1, 1);


  bool attack = oneIn(2); // JG, FIXME: If player is on neighbour tile, we should ALWAYS attack.
  if (attack) {
    logstr log;
    HitCmd(*this, dx, dy).Do(log);
  } else { // walk
    std::stringstream ss;
    bool bLegal = WalkCmd(*this, dx, dy).Do(ss);
  }

  // if (!bLegal) { return duration; } // Even if it failed, throw away our turn.

	return duration;
}





bool MobQueue::dispatchFirst() {
  if (queue.empty()) { debstr() << "mob queue is empty, bailing out.\n"; return false; }

  MobReady cur = queue.top();
  globalClock = cur.when; // update clock to next 'threshold time'.
  // debstr() << "dispatchFirst begin, mob:" << cur.mob->rep() << ", time:" << globalClock << " " << (void*) cur.mob << "\n";
  queue.pop(); // pop_heap / take us out of the queue.

  // Keep doing actions until a non-zero action is done:
  double duration=0;
  for ( ; duration==0 && !cur.mob->isDead(); ) { duration = cur.mob->act(); } 
  // while ((duration = ready.mob->act()) == 0 && !ready.mob) { } 

  if (!cur.mob->isDead()) { // As long as you are not dead, you get a next turn:
    double nextReady = globalClock += duration;
    queue.push(MobReady(nextReady,cur.mob));
  } // if-not-dead.
  // NB, something needs to clean up/delete dead mobs!

  return !cur.mob->isDead(); 
} // dispatchFirst.



MobQueue MobQueue::mobs;



void MobQueue::deleteMob(Mob* toDelete) {

  Map::map[toDelete->pos].creature.clearMob(); // Remove it from the map description.
  toDelete->invalidateGfx(); // Tell graphics system it needs to be redrawn.

  std::vector<Mob*>::iterator i;
  i = std::find(globalMobs.begin(), globalMobs.end(), toDelete);
  if (i != globalMobs.end()) { globalMobs.erase(i); }

  // JG, : I couldn't implement, because I need my own prio-queue-deque-heap to allow removal.
  ReadyQueue::iterator j; 
  for (j = queue.begin(); j != queue.end(); ++j) { // FIXME - is there a better way to search?
    MobReady& mr = *j;
    if (mr.mob == toDelete) { break;  }
  }
  if (j != queue.end()) { 
    queue.erase(j); 
    queue.makeHeap(); // re-establish heap.
    // http://www.linuxtopia.org/online_books/programming_books/c++_practical_programming/c++_practical_programming_189.html
  }
  delete toDelete;
}




std::string MonsterMob::pronoun() const { // { return "you";  } // "You"/"The orc".
  CreatureEnum theCtype = ctype();
  CString s = Creature::ctypeAsDesc(theCtype);
  s.Replace(L".", L"the");

  CT2A asc(s, CP_ACP);
  std::string sAsc = asc;
  return sAsc;
}




PlayerMob* PlayerMob::ply = NULL;
PlayerMob::PlayerMob() { ply = this;  }
PlayerMob::~PlayerMob() { ply = NULL;  }


int PlayerMob::distPly(CPoint p) {
  if (ply == NULL) { return 100;  } // Far away then..
  CPoint delta = p - ply->pos;
  int dist = delta.x*delta.x + delta.y*delta.y;
  return dist;
}



bool Mob::wear(Obj* obj, std::ostream& err) { // Obj will go to/from bag.
  Obj* oldItem = NULL;
  if (!Equ::worn.replaceItem(obj, &oldItem, err)) { return false;  }

  bool bOK = false;
  bOK = Bag::bag.remove(obj, err); // Item must be removed from bag.
  if (!bOK) { return false;  }
  if (oldItem != NULL) { // old item must go back in the bag.
    logstr log;  log << "You put the old item in your bag.";
    bOK = Bag::bag.add(oldItem, err);
  }
  return bOK;
}






bool Mob::hitTest(class Mob& adv, AttackInf& ai) { // int& hitRoll, int hitBonus) { // FIXME: consider passing entire weapon-obj instead of just the hitbonus.
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


 

bool Mob::calcAttack(class Mob& adv, AttackInf& ai, std::ostream& os) { // int& dmg) {
  // Collect 'attack info' in an AttackInfo struct.
  // AttackInf ai;

  Obj* player_weapon = Equ::worn.weapon(); // FIXME, inventory and equipment should be members of Mobs.
  if (isPlayer()) { // ctype() == CR_Player) {
    if (player_weapon != NULL) { ai.wpHitBonus = player_weapon->toHit; }
  }

  ai.bHit = hitTest(adv, ai); // ai.hitRoll, ai.hitBonus);
  if (!ai.bHit) { return false;  }

  ai.attackDice = mobWeaponDice();
  if (isPlayer()) { // ctype() == CR_Player) {
    if (player_weapon != NULL) { 
      ai.attackDice = player_weapon->dmgDice; 
      ai.dmgBonus = player_weapon->toDmg;
    }
   
  }

  ai.dmgRoll = ai.attackDice.roll(os);
  ai.dmgMod = stats.statMod("str"); // You get your strength bonus added to dmg.
  ai.dmg += ai.dmgRoll + ai.dmgMod + ai.dmgBonus;
  AttackSchool type = SC_Phys; 

  ai.dmgTaken = adv.takeDamage(ai.dmg, type); 

  return true;
}




int Mob::takeDamage(int dmg, AttackSchool damageType) { // returns dmgTaken
  int dmgTaken = dmg; // Might be adjusted by resistances or vulnerabilities. (should we calculate this here, or outside in dmg code?)
  stats.hp -= dmgTaken;
  return dmgTaken;
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
