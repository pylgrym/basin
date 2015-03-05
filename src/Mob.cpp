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

#include "Encumb.h"

Mob::Mob(int mlevel, bool bIsPlayer_)
  :stats(mlevel,bIsPlayer_)
{

  // mobDummyWeapon = Dice(rnd(3), rnd(2,12)); // Wow that can hit hard..
  mobDummyWeapon = Levelize::randDiceForLevel(mlevel);

  m_mobType = (CreatureEnum) rnd(CR_Kobold, CR_MaxLimit);

  pos.x = rnd(1, Map::Width-1); 
  pos.y = rnd(2, Map::Height-1);
  color = RGB(rand()%255,rand()%255,rand()%255);
  speed = 1.0;

  defSchool = (AttackSchool) rnd(0, SC_MaxSchools);

  bool badMood = oneIn(12);
  if (badMood) { 
    mood = (MoodEnum) rnd(0, M_MaxMoods);
  } else { // Most monsters start out sleeping, or wandering.
    bool awake = oneIn(6);
    mood = (awake ? M_Wandering : M_Sleeping);
  }
  
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


void addInfS(const char* label, std::string right, int width,  CPoint& dash) {
  std::string row = label;

  //std::stringstream ss; ss << val; std::string right = ss.str();
  std::string filler = "................";

  // Make a row 'FULL WIDTH', by padding with 'filler':
  row = row + filler.substr(0, (width - row.length()));

  // overwrite rightmost part of row with 'right':
  int delta = (width - right.length());
  if (delta < 0) { delta = 0;  }
  row = row.substr(0, delta) + right;

  Cuss::move(dash);
  Cuss::prt(row.c_str(), true);
  dash.y += 1;
}


void addInf1(const char* label, int val, int width,  CPoint& dash) {
  std::stringstream ss; ss << val; std::string right = ss.str();
  addInfS(label, right, width, dash);
}

void addInf2(const char* label, int val1, int val2, int width,  CPoint& dash) {
  std::stringstream ss; ss << val1 << "/" << val2; std::string right = ss.str();
  addInfS(label, right, width, dash);
}


void PlayerMob::dashboard() {
  const int width = 8; // 10;
  /* FIXME - make functions for these.
  */
  CPoint dash(Viewport::Width, 1);
  // std::stringstream ss;

  addInf1("light", lightStrength(), width, dash);
  // ss << std::fixed << std::setw(4) << "light:";  addInf(ss, dash);
  // ss << std::fixed << std::setw(5) << lightStrength(); addInf(ss, dash);

  addInf1("unit", theLightUnits, width, dash);
  // ss << std::fixed << std::setw(5) << this->theLightUnits; addInf(ss, dash);

  //addInf2("hp", stats.hp, width, dash);
  //ss << "" << std::fixed << std::setw(5) << stats.hp << "/" << stats.maxHP << "hp "; addInf(ss, dash); //  << "/" << stats.maxHP << " ";
  addInf2("hp", stats.hp, stats.maxHP, width, dash);

  // ss // << "xp:" << std::fixed << std::setw(5) << stats.xp << "/" << stats.xpToLevel << "xp "; addInf(ss, dash);
  //ss << "mXxp:" << stats.xpToLevel; addInf(ss, dash);
  addInf2("xp", stats.xp, stats.xpToLevel, width, dash);

  addInf1("level", stats.level(), width, dash);
  //ss // << "lvl:"   << std::fixed << std::setw(3) << stats.level() << "L "; addInf(ss, dash);

  int depth = PlayerMob::ply ? PlayerMob::ply->dungLevel : 0;
  addInf1("depth", depth, width, dash);
  // ss // << "ac:"  << std::fixed << std::setw(3) << depth << "dung"; addInf(ss, dash);

  addInf1("ac", stats.ac, width, dash);
  // ss // << "ac:"  << std::fixed << std::setw(3) << stats.ac << "ac"; addInf(ss, dash);
  addInf2("worn", stats.wornAC_input, stats.wornAC_output, width, dash);

  int acEffect = Stats::calcAvgACeffect();
  addInf1("ac%", acEffect, width, dash);
  // ss // << "ac effect:"  << std::fixed << std::setw(3) << acEffect << "%ac"; addInf(ss, dash);

  // FIXME, should be recalculated per turn, and possibly stored in stats:
  // FIXME, show as text
  // FIXME, pick up should check this!
  Encumb::EncumbEnum encumb = Encumb::enc(); // Encumb::EncumbEnum 
  const char* sEncumb = Encumb::encTxt(encumb); // encu
  // ss << "W:" << sEncumb; //  std::fixed << std::setw(2) << encumb;
  addInfS("enc", sEncumb, width, dash); // (ss, dash);

  addInf1("food", stats.hunger, width, dash);
  addInf1("conf.?", (int) stats.isConfused(), width, dash);
  addInfS("", stats.isConfused() ? "confused" : "-", width, dash);

}




double PlayerMob::act() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  double actionDuration = 1.0; // seconds. // WalkCmd/Cmd's might set this instead.

  bool bActionDone = false;
  for (;!bActionDone && !TheUI::hasQuit;) { // JG: this seems to have been the important one, of 'hasQuit' checks..
    passTime(); // Step the time, for 'things that happen every N seconds', e.g. hunger. FIXME - probably should respect action-duration.

	  // Prompt user for command, then move:
	  int nChar = TheUI::getNextKey(__FILE__, __LINE__, "user-choose-action"); // in PlayerMob::act.

    LogEvents::log.resetNotif();

    // Clear any left-over msg:
    Cuss::move(CPoint(0, 0));
    Cuss::prtL("");

    bool bShift = TheUI::shiftKey(); // Shift is running, but doesn't exist yet.
    bool bCtrl = TheUI::ctrlKey();

    // std::stringstream ss;
    logstr ss;

    switch (nChar) {
    case ' ': { actionDuration = 0; bActionDone = false; } break; // space shouldn't do anything.
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

    case VK_ADD:      if (DexModCmd(+1).Do(ss)) { actionDuration = 1; bActionDone = true; } break; 
    case VK_SUBTRACT: if (DexModCmd(-1).Do(ss)) { actionDuration = 1; bActionDone = true; } break; 


    case 'S': 
      if (bCtrl) {
        if (SaveCmd().Do(ss)) { actionDuration = 0; bActionDone = true; } break;
      } else {
        if (LookCmd(*this).Do(ss)) { actionDuration = 0; bActionDone = true; } break;
      }

    case 'Z': if (ZapCmd(NULL, *this, SP_FireBolt, SC_Holy).Do(ss))  { actionDuration = 1; bActionDone = true; } break; 

    case 'Q': 
      if (bCtrl) {
        if (LoadCmd().Do(ss)) { actionDuration = 0; bActionDone = true; } break;
      } else {
        // Didn't this use-to-be statscmd?
        if (StatCmd().Do(ss)) { actionDuration = 0; bActionDone = true; } break;
        //if (LookCmd(*this).Do(ss)) { actionDuration = 0; bActionDone = true; } break;
      }

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

std::string MonsterMob::a_mob() const { // { return "you";  } // "You"/"An orc".
  CreatureEnum theCtype = ctype();
  std::string s = Creature::ctypeAsDesc(theCtype); 
  CA2T us(s.c_str(), CP_ACP);

  // The lack of 'replace' on std::string sucks.
  CString s2 = us; 
  s2.Replace(L".", L"a");

  CT2A asc(s2, CP_ACP);
  std::string sAsc = asc;
  return sAsc;
}




PlayerMob* PlayerMob::ply = NULL;

PlayerMob::PlayerMob():Mob(1, true) { 
  ply = this;  

  dungLevel = 0; // 1;
  pos.x = 15; 
  pos.y = 15; // FIXME; must be free!

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
  Obj* lamp = findLight();
  if (lamp != NULL) {
    int activeStr = lamp->getLightStrength();
    if (lamp->itemUnits == 0) { activeStr = 1; } // else { activeStr *= 1;  }
    setLightStrength(activeStr, lamp->itemUnits); // We just cache the units to hud-dashboard-monitor them.

    bool burnout = false;
    if (lamp->itemUnits == 1) { burnout = true; } // Is it the last flicker, burning out now?
    lamp->burnUnits(1);
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
  debstr() << "after replaceItem, old:" << (void*) oldItem << "\n";

  bool bOK = false;
  bOK = Bag::bag.remove(obj, err); // Item must be removed from bag.
  if (!bOK) { return false;  }
  if (oldItem != NULL) { // old item must go back in the bag.
    logstr log; log << "You put the old item in your bag.";
    bOK = Bag::bag.add(oldItem, err);
  } else {
    // Beware that weapon-unequip will handle stuff by itself (swapping or putting in bag.)
    //logstr log; log << "No old item..";
  }
  return bOK;
}






 
















PlayerMob* PlayerMob::createPlayer() {
  PlayerMob* player = new PlayerMob; // Is a singleton, will store himself.

  std::stringstream ignore;
  Bag::bag.add(new Obj(Obj::objDesc(OB_Hat), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Sword), 1), ignore);

  // Bag::bag.add(new Obj(OB_Gold),ignore);
  Obj* firstLamp = new Obj(Obj::objDesc(OB_Lamp), 1);
  firstLamp->itemUnits = 300;
  Bag::bag.add(firstLamp, ignore);

  Bag::bag.add(new Obj(Obj::objDesc(OB_Hat), 1), ignore);

  Bag::bag.add(new Obj(Obj::objDesc(OB_Potion), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Scroll), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Food), 1), ignore);
  Bag::bag.add(new Obj(Obj::objDesc(OB_Pickaxe), 1), ignore);

  Obj* oil1 = new Obj(Obj::objDesc(OB_LampOil),1);
  oil1->itemUnits = 900;
  Bag::bag.add(oil1, ignore);

  Obj* oil2 = new Obj(Obj::objDesc(OB_LampOil), 1);
  oil2->itemUnits = 2900;
  Bag::bag.add(oil2, ignore);

  // IMPORTANT: once inventory has weight in bag, we must recalc for encumbrance stats!
  PlayerMob::ply->stats.calcStats();


  // Create shop - shouldn't be in this function I think..
  Bag::shop.add(new Obj(Obj::objDesc(OB_Food), 1), ignore);
  Bag::shop.add(new Obj(Obj::objDesc(OB_Pickaxe), 1), ignore);
  Obj* firstLamp2 = new Obj(Obj::objDesc(OB_Lamp), 1);
  firstLamp2->itemUnits = 3700;
  Bag::shop.add(firstLamp, ignore);
  Bag::shop.add(new Obj(Obj::objDesc(OB_LampOil), 1), ignore);



  return player;
}

