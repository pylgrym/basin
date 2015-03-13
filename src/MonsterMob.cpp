#include "stdafx.h"
#include "MonsterMob.h"
#include "Mob.h"
#include "Cmds.h"
#include "PlayerMob.h"

const bool MLog = false; // ML is 'monster log/verbose output'.

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
    if (MLog) { debstr() << "I stay asleep.\n"; } 
  }

  return 1.0; // duration.
}



double MonsterMob::actWander() { // returns time that action requires (0 means keep doing actions/keep initiative.)
	// stagger to a random location:
  if (MLog) { debstr() << "I wander around randomly.\n"; }
  int dx = rndC(-1, 1), dy = rndC(-1, 1);
  std::stringstream ss;
  bool bLegal = WalkCmd(*this, dx, dy, false).Do(ss);
  return 1.0; // duration.
}


// abs is in stdlib.h
//int abs(int a) { return (a > 0 ? a : -a); } // FIXME, move to num-utils.


bool Mob::playerOnStar() const { // If player is on a '8-star direction', we can use spell against him.

  CPoint delta = playerDelta();  // ply - pos;

  if (delta.x == 0 || delta.y == 0) { return true; } // .x == ply.x || pos.y = ply.y)
  if (abs(delta.x) == abs(delta.y))  { return true;  }
  // CPoint ply = PlayerMob::ply->pos;

  // IDEA: spells can have ranges - require touch, or NOT allow directly next to target, or max 4 steps.

  return false;
}



double MonsterMob::actAngry() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  CPoint dir = playerDir();

  if (playerOnStar()) {
    // if (nearPlayer()) { will change prob. (longer range, higher chance of spell attack.)
    int castChance = (nearPlayer() ? 7 : 33);
    bool willCast = pctChance(castChance);
    if (willCast) {
      ZapCmd cmd(NULL, *this, mobSpell, this->defSchool); // FIXME, should be spell's school instead? 
      cmd.mobZapDir = dir; // Interesting/idea: this way, a mob can do 'friendly fire'/another mob can be caught in crossfire!

      // not a_mob
      { logstr log; log << "Oh no! The " << this->pronoun() << " is aiming a spell at you!"; }
      logstr log; // Will show a mob attacking!
      bool bOK = cmd.Do(log);
      if (bOK) { return 1.0; } // At least he used up his turn now..
    }

  }

  if (nearPlayer()) {
    debstr() << "I am near player and will attack!\n";
    // JG, If player is on neighbour tile, we should ALWAYS attack.
    logstr log;
    HitCmd(NULL, *this, dir.x, dir.y, SC_Phys, SP_NoSpell).Do(log); // FIXME: monsters should have a preferred attack type..
  } else { // Else, chase the player:
    if (MLog) { debstr() << "I am far from player and will chase!\n"; }
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

CPoint Mob::playerDelta() const {
  CPoint delta = (PlayerMob::ply->pos - pos);
  return delta;
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

  if (MLog) { debstr() << "I am afraid and will flee from player.\n"; }

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
            HitCmd(NULL, *this, dir.x, dir.y, SC_Phys, SP_NoSpell).Do(ss);  
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



std::string  MonsterMob::a_mob() const { // { return "you";  } // "You"/"An orc".
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



bool hurt() { return false; }
bool can_flee() { return false; }
bool flee_prob() { return false; }
bool flee() { return false; }

bool can_attack() { return false; }
bool hurt_attack_prob() { return false; }
bool attack() { return false; }

bool too_close() { return false; }
bool can_incr() { return false; }
bool incr_prob() { return false; }
bool incr_dist() { return false; }

bool too_far() { return false; }
bool can_decr() { return false; }
bool decr_prob() { return false; }
bool decr_dist() { return false; }

bool melee_range() { return false; }
bool melee_prob() { return false; }
bool attack_melee() { return false; }

bool can_ranged() { return false; }
bool ranged_prob() { return false; }
bool attack_ranged() { return false; }

bool stay() { return false; }


double MonsterMob::actGeneric() {
  return 1.0;
}

void f_ai() {
  /* todo - compare with article http://www.roguebasin.com/index.php?title=Roguelike_Intelligence_-_Stateless_AIs
  to understand if I'm missing out on features he manages to fit in.
  */
  if (hurt()) { // (morale-check)
    if (can_flee() && flee_prob()) { flee(); return; }
    if (can_attack() && hurt_attack_prob()) { attack(); return; }
    stay(); // Stay may involve spellcast/healing/alerting.
  } else { // Not hurt.
    if (too_close() && can_incr() && incr_prob() ) { incr_dist(); return; }
    if (too_far() && can_decr() && decr_prob() ) { decr_dist(); return; }
    if (melee_range() & melee_prob()) { attack_melee(); return; }
    if (can_ranged() & ranged_prob()) { attack_ranged(); return; }
    stay();
  }
}

/* ai info:
http://www.roguebasin.com/index.php?title=Roguelike_Intelligence_-_Stateless_AIs

Priorities here:
(1) (morale%) if hurt, flee-else-attack
(2) (charge%,retreat%,min/max-range) if undesired-range and 'have-options': randomly attack or adjust-range.
(3) if can-attack: attack
(4) if undesired-range and can-adjust: adjust.
(5) remain.

        TYPICAL AI

If damage > morale {
  if can-run-away-from-player {
    run-away-from-player 
  } else if can-attack-player {
    attack-player
  }

} else if too-far-from-player AND can-attack-player AND can-move-toward-player {
  if random < charge-probability {
    move-toward-player     
  } else  {
    attack-player
  }

} else if too-close-to-character AND can-attack-player AND can-move-away-from-player {
  if random < retreat-probability {
    move-away-from-player
  } else {
    attack-player
  }

else if can-attack-player {
  attack-player

} else if too-far-from-player AND can-move-toward-player {
  move-toward-player

} else if too-close-to-player AND can-move-away-from-player {
  move-away-from-player

} else {
  stand-still
}

*/



void putpixel(int x, int y, int color){}

void bres_line(int x,int y,int x2, int y2, std::vector<CPoint>& pixels) { // int color) {
  // Bresenham.
  // Alternatively, this should be a visitor-iterator, where 'putpixel' becomes the callback.
  // Source: http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
  int w = x2 - x; 
  int h = y2 - y; 
  int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0; 
  if (w<0) { dx1 = -1; } else if (w>0) { dx1 = 1; }
  if (h<0) { dy1 = -1; } else if (h>0) { dy1 = 1; }
  if (w<0) { dx2 = -1; } else if (w>0) { dx2 = 1; }
  int longest = abs(w); 
  int shortest = abs(h); 
  if (!(longest>shortest)) {
    longest = abs(h); 
    shortest = abs(w); 
    if (h<0) { dy2 = -1; } else if (h>0) { dy2 = 1; }
    dx2 = 0;             
  }
  int numerator = longest >> 1; 
  for (int i=0; i<=longest; i++) {
    pixels.push_back(CPoint(x, y)); // putpixel(x, y, color);
    numerator += shortest;
    if (!(numerator<longest)) {
        numerator -= longest; 
        x += dx1; y += dy1;
    } else {
        x += dx2; y += dy2;
    }
  }
}



class BresIter {
public:
  std::vector<CPoint> pixels; // Output.
  int i; // loop var.

  int longest, shortest;
  int dx1, dy1, dx2, dy2;
  int numerator;
  int x, y;

  BresIter(int x1, int y1, int x2, int y2) {
    // Bresenham.
    // Alternatively, this should be a visitor-iterator, where 'putpixel' becomes the callback.
    // Source: http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
    x = x1; y = y1;

    int w = x2 - x;
    int h = y2 - y;

    dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;
    if (w < 0) { dx1 = -1; }
    else if (w > 0) { dx1 = 1; }
    if (h < 0) { dy1 = -1; }
    else if (h > 0) { dy1 = 1; }
    if (w < 0) { dx2 = -1; }
    else if (w > 0) { dx2 = 1; }
    longest = abs(w);
    shortest = abs(h);
    if (!(longest > shortest)) {
      longest = abs(h);
      shortest = abs(w);
      if (h < 0) { dy2 = -1; }
      else if (h > 0) { dy2 = 1; }
      dx2 = 0;
    }
    numerator = longest >> 1;

    i = 0;
  }

  void iterAll() {
    for (int i=0; i<=longest; i++) {
      next();
    }
  }

  void iterAll2() {
    for ( ; !done(); ) {
      CPoint p = next();
      // Use p
    } 

    do { CPoint p = next(); } while (!done());
  }

  bool done() const { return !(i <= longest);  }

  CPoint next() {
    CPoint curPoint(x, y);
    pixels.push_back(curPoint); // putpixel(x, y, color);

    numerator += shortest;
    if (!(numerator<longest)) {
      numerator -= longest; 
      x += dx1; y += dy1;
    } else {
      x += dx2; y += dy2;
    }

    ++i;
    return curPoint;
  }

}; // end class BresIter 



void bresenExample() {
  BresIter i(4, 9, 74, 35);
  for ( ; !i.done(); ) {
    CPoint p = i.next();
    CL->map[p].envir.type = EN_Green;
    // Use p
  } 
}
