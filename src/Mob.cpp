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
#include "Levelize.h"

Mob::Mob(int mlevel)
  :stats(mlevel)
{

  // mobDummyWeapon = Dice(rnd(3), rnd(2,12)); // Wow that can hit hard..
  mobDummyWeapon = Levelize::randDiceForLevel(mlevel);

  m_mobType = (CreatureEnum) rnd(CR_Kobold, CR_MaxLimit);

  pos.x = rnd(1, Map::Width-1); 
  pos.y = rnd(2, Map::Height-1);
  color = RGB(rand()%255,rand()%255,rand()%255);
  speed = 1.0;

  defSchool = (AttackSchool) rnd(0, SC_MaxSchools);

  mood = (MoodEnum) rnd(0, M_MaxMoods);
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











void PlayerMob::passTime() {
	stats.passTime();
	updateLight();
	dashboard();
}

void addInf(std::stringstream& ss, CPoint& dash) {
  Cuss::move(dash);
  std::string lightInf = ss.str();
  Cuss::prt(lightInf.c_str(), true);
  ss.str("");
  dash.y += 1;
}

void PlayerMob::dashboard() {
  /* FIXME - make functions for these.
  */
  CPoint dash(Viewport::Width, 1);
  std::stringstream ss;

  ss << std::fixed << std::setw(4);

  ss << "light:";  addInf(ss, dash);
  ss << "#"   << std::fixed << std::setw(3) << lightStrength() << "#"; addInf(ss, dash);
  ss << "#"   << std::fixed << std::setw(4) << this->theLightUnits << "  "; addInf(ss, dash);
  ss << "hp:" << std::fixed << std::setw(3) << stats.hp << "#"; addInf(ss, dash); //  << "/" << stats.maxHP << " ";

  ss << "xp:" << std::fixed << std::setw(5) << stats.xp; addInf(ss, dash);
  ss << "mXxp:" << stats.xpToLevel; addInf(ss, dash);
  ss << "lvl:" << stats.level(); addInf(ss, dash);

  ss << "ac:" << std::fixed << std::setw(3) << stats.ac; addInf(ss, dash);

}




double PlayerMob::act() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  double actionDuration = 1.0; // seconds. // WalkCmd/Cmd's might set this instead.

  bool bActionDone = false;
  for (;!bActionDone;) {
    passTime(); // Step the time, for 'things that happen every N seconds', e.g. hunger. FIXME - probably should respect action-duration.

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
    case 'E': if ( ExamineCmd(*this).Do(ss)) { actionDuration = 0; bActionDone = true; } break; // Use is.. A..?

    case 'D': if (DropCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true; } break; // D is DROP.
    case 'G': if (TakeCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true;  } break; // G is GET.
    case 'I': if (InvCmd().Do(ss))       { actionDuration = 0; bActionDone = true; } break; // I is INVentory (not B-bag.)
    case 'P': if (bCtrl) { if (ShowEventsCmd().Do(ss)) { actionDuration = 0; bActionDone = true; } break; } // ctrl-P is 'show event log'.
    case 'T': if (UnequipCmd().Do(ss))   { actionDuration = 1; bActionDone = true; } break; // T is 'take-off', as U=unequip is already used.

    case '.': 
    case VK_OEM_PERIOD: if (WaitCmd(*this).Do(ss)) { actionDuration = 1; bActionDone = true; } break; 

    case 'S': 
      if (bCtrl) {
        if (SaveCmd().Do(ss)) { actionDuration = 0; bActionDone = true; } break;
      } else {
        if (LookCmd(*this).Do(ss)) { actionDuration = 0; bActionDone = true; } break;
      }

    case 'Z': if (ZapCmd(NULL, *this, SP_FireBolt, SC_Holy).Do(ss))  { actionDuration = 0; bActionDone = true; } break; 

    case 'Q': if (StatCmd().Do(ss))       { actionDuration = 0; bActionDone = true; } break; // Q is stats (should it be S?)

    case 'C': if (StairCmd(*this).Do(ss))       { actionDuration = 1; bActionDone = true; } break; // C is upstairs/downstairs.


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

        if (CL->map[target].creature.empty()) {
          if (bCtrl) { // CTRL means digging:
            if (DigCmd(*this, dx, dy).Do(ss)) { actionDuration = 1; bActionDone = true; }
          } else { // no control-key - it's a move.
            if (WalkCmd(*this, dx, dy, false).Do(ss)) { actionDuration = 1; bActionDone = true; }
          }
        } else { // target-field HAS a creature - then it's an attack (we aren't very social, are we..)
          Obj* player_weapon = Equ::worn.weapon(); 
          if (HitCmd(player_weapon, *this, dx, dy, SC_Phys).Do(ss)) { actionDuration = 1; bActionDone = true; }
        }
      } //end movement-scope-block.
      break; // end of keyboard switch.
    } // end switch keyboard.

  } // wait-for-successful-command loop.

	return actionDuration;
}





double MonsterMob::act() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  double duration = 1.0; // actionDuration  // seconds.

  switch (mood) {
  case M_Sleeping:  duration = actSleep(); break;
  case M_Wandering: duration = actWander(); break;
  case M_Angry:     duration = actAngry(); break;
  case M_Afraid:    duration = actFlee(); break;
  }


	return duration;
}


double MonsterMob::actSleep() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  bool wake = oneIn(10); 
  if (wake) {
    logstr log; log << "Something awakens!";
    bool angry = oneIn(2);  
    mood = angry ? M_Angry : M_Wandering;
  } else {
    debstr() << "I stay asleep.\n"; // on.";
  }

  return 1.0; // duration.
}

double MonsterMob::actWander() { // returns time that action requires (0 means keep doing actions/keep initiative.)
	// stagger to a random location:
  debstr() << "I wander around randomly.\n";
  int dx = rndC(-1, 1), dy = rndC(-1, 1);
  std::stringstream ss;
  bool bLegal = WalkCmd(*this, dx, dy, false).Do(ss);
  return 1.0; // duration.
}


double MonsterMob::actAngry() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  CPoint dir = playerDir();
  if (nearPlayer()) {
    debstr() << "I am near player and will attack!\n";
    // JG, If player is on neighbour tile, we should ALWAYS attack.
    logstr log;
    HitCmd(NULL, *this, dir.x, dir.y, SC_Phys).Do(log); // FIXME: monsters should have a preferred attack type..
  } else { // Else, chase the player:
    debstr() << "I am far from player and will chase!\n";
    std::stringstream ss;
    WalkCmd walk(*this, dir.x, dir.y, false);
    if (!walk.legal(ss)) { // If moving straight across is blocked..
      CPoint dirV = dir, dirH = dir;
      dirV.x = 0; dirH.y = 0;
      walk.newpos = (pos + dirV); // what about going vertical?
      if (!walk.legal(ss)) { // if vertical doesn't work, what about horizontal?
        walk.newpos = (pos + dirH);
      }
    }
    bool bLegal = walk.Do(ss);
  }

  if (lowHealth()) {
    if (oneIn(3)) {
      logstr log; log << "The monster flees, feeling hurt.";
      mood = M_Afraid;
    }
  }

  return 1.0; // duration.
}



void Mob::makeAngry() {
  if (mood == M_Sleeping || mood == M_Wandering) {
    mood = M_Angry;
    logstr log; log << "You have made the monster angry.";
  }
}



bool Mob::lowHealth() const {
  bool bLow = (stats.hp < (2*stats.maxHP/5) ); // Less than 2/5's
  return bLow;
}


double MonsterMob::actFlee() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  CPoint dir = playerDir();
  dir.x = -dir.x; dir.y = -dir.y; // Flee in the opposite dir.

  debstr() << "I am afraid and will flee from player.\n";

  std::stringstream ss;
  bool bLegal = WalkCmd(*this, dir.x, dir.y, false).Do(ss);
  if (!bLegal) { // if we can't flee in that dir, try a random dir:
	  // stagger to a random location:
    int dx = rndC(-1, 1), dy = rndC(-1, 1);
    bool bLegal = WalkCmd(*this, dx, dy, false).Do(ss);
  }

  return 1.0; // duration.
}














std::string MonsterMob::pronoun() const { // { return "you";  } // "You"/"The orc".
  CreatureEnum theCtype = ctype();
  std::string s = Creature::ctypeAsDesc(theCtype); // CString s = 
  CA2T us(s.c_str(), CP_ACP);

  // The lack of 'replace' on std::string sucks.
  CString s2 = us; // us = s;
  s2.Replace(L".", L"the");
  // std::replace(s.begin(), s.end(), ".", "the");

  CT2A asc(s2, CP_ACP);
  std::string sAsc = asc;
  return sAsc;
}




PlayerMob* PlayerMob::ply = NULL;

PlayerMob::PlayerMob():Mob(1) { 
  ply = this;  
  dungLevel = 1;
  theLightStrength = 1;
  theLightUnits = 0;
  m_mobType = CR_Player; // (CreatureEnum)rnd(CR_Kobold, CR_MaxLimit);
}

PlayerMob::~PlayerMob() { ply = NULL;  }


int PlayerMob::distPly(CPoint p) {
  if (ply == NULL) { return 100;  } // Far away then..
  CPoint delta = p - ply->pos;
  int dist = delta.x*delta.x + delta.y*delta.y;
  return dist;
}

int PlayerMob::distPlyLight(CPoint p) {
  int dist = distPly(p);

  // Clip dist, for stronger torch:
  int strength = ply->lightStrength();
  if (strength > 0) {
    dist = dist / strength;
  } else {
    dist *= 5; // or set it to 'far away'?
  }

  //dist = dist - 2; 
  return dist;
}

void PlayerMob::updateLight() {
  Obj* light = findLight();
  if (light != NULL) {
    int activeStr = light->getLightStrength();
    if (light->itemUnits == 0) { activeStr = 1; } else { activeStr *= 1;  }
    setLightStrength(activeStr, light->itemUnits); 

    bool burnout = false;
    if (light->itemUnits == 1) { burnout = true;  } // Is it the last flicker, burning out now?
    light->burnUnits(1);
    if (burnout) { logstr log; log << "Your light flickers out!"; }

  } else {
    setLightStrength(rnd(0,2),0);  // Flicker-torch.
  }
}


Obj* PlayerMob::findLight() { // FIXME: You'll get a random lamp, maybe not the strongest we are carrying..
  Obj* obj = Bag::bag.findItem(OB_Lamp);
  return obj; // May  be NULL.
}

// Obj* PlayerMob::findShovel() { // FIXME: You'll get a random shovel
//  Obj* obj = Bag::bag.findItem(OB_Pickaxe);
//  return obj; // May  be NULL.
//}


int PlayerMob::digStrength() {
  Obj* weapon = Equ::worn.weapon();
  if (weapon == NULL) {
    int strMod = stats.statMod("str");
    if (strMod < 1) { strMod = 1;  }
    logstr log; log << "You dig with your bare hands, barely making progess.";
    return strMod;
  }

  if (weapon->otype() == OB_Pickaxe) {
    logstr log; log << "Your pickaxe cuts into the rock.";
    return weapon->digStrength();
  }

  //logstr log; log << "You try to dig into the rock with your weapon.";
  logstr log; log << "You try to dig with your weapon, clumsily.";
  return weapon->digStrength();
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


bool Mob::nearPlayer() const {
  int dist = PlayerMob::distPly(pos);
  return (dist <= 1);
}

CPoint Mob::playerDir() const {
  CPoint delta = (PlayerMob::ply->pos - pos);
  CPoint dir;
  if (delta.x > 0) { dir.x = 1; }
  if (delta.x < 0) { dir.x = -1; }
  if (delta.y > 0) { dir.y = 1; }
  if (delta.y < 0) { dir.y = -1; }
  return dir;
}




PlayerMob* PlayerMob::createPlayer() {
  PlayerMob* player = new PlayerMob; // Is a singleton, will store himself.

  std::stringstream ignore;
  Bag::bag.add(new Obj(Obj::objDesc(OB_Hat), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Sword), 1), ignore);

  // Bag::bag.add(new Obj(OB_Gold),ignore);
  Obj* firstLamp = new Obj(Obj::objDesc(OB_Lamp), 1);
  firstLamp->itemUnits = 3700;
  Bag::bag.add(firstLamp, ignore);

  Bag::bag.add(new Obj(Obj::objDesc(OB_Hat), 1), ignore);

  Bag::bag.add(new Obj(Obj::objDesc(OB_Potion), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Scroll), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Food), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Pickaxe), 1), ignore);
  return player;
}

