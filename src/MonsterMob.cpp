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
