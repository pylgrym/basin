#include "stdafx.h"
#include "MonsterMob.h"
#include "Mob.h"
#include "Cmds.h"
#include "PlayerMob.h"


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


/* Because these risks are small chances (percent, promille..), we don't want to do crude 5% D20 tests. */
const double noticeDSize = 1000.0;

// descs should be shorter for 48 chars, so leave out 'head, shoulder'etc
double Mob::noticePlayerProb(CPoint coords, int mobAlert) {
  /* dex 11 stealth seemed to be ok at the outset - 4th tile away, 1%. dex 38 is what we want at 18
     dex -6 is what we want for 3.
  */

  // NB! These are 1/20=5%'s, must be adjusted.
  int plyStealth = PlayerMob::ply->stats.stealth();
  //int mobAlert = 0; // Now an arg.
  double noticeBalance = mobAlert - plyStealth;
  // we want (18-3 = 15) to match (38- -6=44), ie 15 to 44, ie *3.
  noticeBalance = 3*noticeBalance; // we want the stealth/alert stat to have higher jumps, and offset it a bit.
  // Consider: just using an y=ax+b may not be enough to give a good balance.

  double dist = PlayerMob::distPly(coords);
  dist = (dist*dist) / 3.5;

  if (noticeBalance > 0) {
    dist /= noticeBalance;
  } else if (noticeBalance < 0) {
    dist *= (-noticeBalance);
  }

  int baseNoticeProb = int(0.5 + noticeDSize / (dist + 1.0)); // By accident, 1/x actually gives good default chances/distribution!
  //return baseNoticeProb;

  int noticeThreshold = baseNoticeProb; // nt(0.5 + baseNoticeProb + (noticeBalance * (noticeDSize / 20.0))); // it's 5%-ratings, which we convert to promille.

  return noticeThreshold;
}

bool Mob::noticePlayer(double& noticeChance) {
  noticeChance = noticePlayerProb( pos, stats.alertness() ); 
  int roll = Dx( (int) noticeDSize);
  bool noticed = (roll <= noticeChance);  
  return noticed;
}


double MonsterMob::actSleep() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  double chance = 0.0;
  bool wakeUp = noticePlayer(chance); // PlayerMob::ply); // oneIn(10); // FIXME: distance to player, and player-dex, should govern risk of monster disturbed!

  if (wakeUp) {
    {
      logstr log; log << "Something awakens!";
    }
    {      
      logstr log; log << chance << ":" << a_mob() << " notices you!";
    }
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
      // monster 
      logstr log; log << "The " << pronoun() << " flees, feeling hurt.";
      mood = M_Afraid;
    }
  }

  return 1.0; // duration.
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
  WalkCmd walk(*this, dir.x, dir.y, false);
  if (!walk.legal(ss)) { // if we can't flee in that dir, try a random dir:

    // try to stagger to a random location:
    int dx = rndC(-1, 1), dy = rndC(-1, 1);
    walk.newpos = (pos + CPoint(dx, dy));
    if (!walk.legal(ss)) {

      CPoint dirV = dir, dirH = dir;
      dirV.x = 0; dirH.y = 0;
      walk.newpos = (pos + dirV); // what about going vertical?
      if (!walk.legal(ss)) { // if vertical doesn't work, what about horizontal?
        walk.newpos = (pos + dirH);
        if (!walk.legal(ss)) { // if horizontal doesn't work, what about fighting back..
          if (nearPlayer()) {
            logstr log; log << "Cornered, the scared monster fights back!";
            HitCmd(NULL, *this, dir.x, dir.y, SC_Phys).Do(ss);  
            return 1.0;
          }
        }
      }

    }
  }
  bool bLegal = walk.Do(ss);

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
