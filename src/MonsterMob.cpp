#include "stdafx.h"
#include "MonsterMob.h"

#include "Mob.h"

#include "Cmds.h"


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


double Mob::noticePlayerProb(CPoint coords) {
  int dist = PlayerMob::distPly(coords);
  return 1.0/(dist+1.0);

  int plyStealth = PlayerMob::ply->stats.stealth();
  int mobAlert = 0;
  int avoidBalance = plyStealth - mobAlert;
  int threshold = dist + avoidBalance;

  // Now calc percent-prob of threshold and D20.
  double pct = 1.0 - (threshold / 20.0);
  int intPct = int(pct*100.0 + 0.5);
  return intPct;
}


bool Mob::noticePlayer() {  
  /* We are actually checking player's ability to stealth, not mob's ability to notice (they are inverse).
  */
  int dist = PlayerMob::distPly(pos);

  int plyStealth = PlayerMob::ply->stats.stealth();
  int mobAlert = stats.alertness();

  int avoidBalance = plyStealth - mobAlert;
  int noticeBalance = mobAlert - plyStealth;

  int threshold = dist + avoidBalance;

  int roll = Dx(20);

  bool avoid = (roll <= threshold);
  bool notice = !avoid;

  return notice;
}


double MonsterMob::actSleep() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  bool wakeUp = noticePlayer(); // PlayerMob::ply); // oneIn(10); // FIXME: distance to player, and player-dex, should govern risk of monster disturbed!

  if (wakeUp) {
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
