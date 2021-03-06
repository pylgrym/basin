#include "stdafx.h"
#include "MonsterMob.h"
#include "Mob.h"
#include "Cmds.h"
#include "PlayerMob.h"

const bool MLog = false; // ML is 'monster log/verbose output'.

double MonsterMob::act() { // returns time that action requires (0 means keep doing actions/keep initiative.)

  // NB - time should actually pass, independently of mob's speed.
  passTime(); // Step the time, for 'things that happen every N seconds', e.g. hunger. 

  double duration = 1.0; // actionDuration  // seconds.

  switch (mood) {
  case M_Sleeping:  duration = actSleep();  break;
  case M_Wandering: duration = actWander(); break;
  case M_Angry:     duration = actAngry();  break;
  case M_Afraid:    duration = actFlee();   break;
  case M_Driven:    duration = actDriven(); break; // a more complex ai.
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

  int noticeThreshold = baseNoticeProb; // nt(0.5 + baseNoticeProb + (noticeBalance * (noticeDSize / 20.0))); // it's 5%-ratings, which we convert to promille.

  return noticeThreshold;
}

bool Mob::noticePlayer(double& noticeChance) {
  noticeChance = noticePlayerProb( pos, stats.alertness() ); 
  int roll = rnd::Dx( (int) noticeDSize);
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
    bool angry = rnd::oneIn(2);  
    mood = angry ? M_Angry : M_Wandering;
  } else {
    if (MLog) { debstr() << "I stay asleep.\n"; } 
  }

  return 1.0; // duration.
}



double MonsterMob::actWander() { // returns time that action requires (0 means keep doing actions/keep initiative.)
	// stagger to a random location:
  if (MLog) { debstr() << "I wander around randomly.\n"; }
  int dx = rnd::rndC(-1, 1), dy = rnd::rndC(-1, 1);
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


bool Mob::canSee(CPoint b, bool onlyEnvir) {
  return CL->map.canSee(pos, b, onlyEnvir);
}

bool Mob::canSeePlayer() {
  return canSee(PlayerMob::ply->pos, true);
}


std::string Mob::pickAbility() {
  const MobDef& def = this->mobDef();
  int count = def.abilities.size();
  assert(count > 0);
  int choice = rnd::Rnd(0, count);
  std::string ability = def.abilities[choice];
  return ability;
}

bool Mob::mobCasts(CPoint dir) {
  /* FIxME/ consider: mobs start with zapcmd instead of Spell::castSpell.
  / Main differences are
    - that zapcmd works with 'cast/bullet dir' from the get-go (and won't accidentally prompt USER for mob's attack-dir..)
    - zapcmd won't eat mana.
    Apart from that, mobs ought to switch to castSpell, so they e.g. can cast 'improve myself/buff' spells.
  At the core of it, zapcmd shouldn't be a command..
  */

  /* focus blast: a spell that hits exactly 2 spaces away, and hits hard. may require los free.
  also possible cooldown. the idea is, that it takes 'logistics' to navigate the adversary in place.
  strike of opportunity?
  */
  std::string ability = pickAbility();
  { 
    SpellImpl* spellPtr = SpellImpl::spellFromTag(ability);
    logstr log; // log << pronoun() << " aims a spell at you!"; 
    log << pronoun() << " " << ability << " a spell at you!" << spellPtr; 
    // FIXME - make a leaner version  of 'castSpell'.
    if (spellPtr != NULL) {
      SpellParam param;
      class Obj* noItem = NULL;
      Spell::prepareSpell2(param, spellPtr, *this, PlayerMob::ply, noItem);  
      /* JG: intention - prepareSpell2 is supposed to be a 'switch-less' parameter-gatherer ('just gather ALL the things'..)
      Presumably means, that SpellImpl must have some virtual overridable to gather spell-specific params, e.g. a radius or range  or strength of a given spell.
      */
    }
  }

  bool bOK = true;
  if (true) { // new approach.
    Mob* target = PlayerMob::ply; 
    bOK = Spell::castSpell(mobSpell, *this, target, NULL, NoMana);
  } else { // old approach:
    ZapCmd cmd(NULL, *this, mobSpell, this->defSchool); // Monster's spell-cast. FIXME, should be spell's school instead? 
    cmd.mobZapDir = dir; // Interesting/idea: this way, a mob can do 'friendly fire'/another mob can be caught in crossfire!
    logstr log; // Will show a mob attacking!
    bOK = cmd.Do(log);
  }

  return bOK;
}


bool Mob::mobMeleesPlayer(CPoint dir) {
  logstr log;
  const bool dontOverrideHit = false;
  HitCmd mobHits(NULL, *this, dir.x, dir.y, SC_Phys, SP_NoSpell,dontOverrideHit);
  return mobHits.Do(log); // FIXME: monsters should have a preferred attack type..
}


bool Mob::canPass(CPoint newpos) {
  if (!CL->map.legalPos(newpos)) { return false; }
  if (CL->map[newpos].blocked()) { return false; }
  return true;
}

bool Mob::canFlee(CPoint target) {
  CPoint delta = (pos - target);
  CPoint dir = normDir(delta);

  CPoint p1 = (pos + dir);
  if (canPass(p1)) { return true; }
  CPoint dirV = dir, dirH = dir;
  dirV.x = 0; dirH.y = 0;

  p1 = (pos + dirV);
  if (canPass(p1)) { return true; }

  p1 = (pos + dirH);
  if (canPass(p1)) { return true; }

  return false;
}

bool Mob::canChase(CPoint target) {
  CPoint delta = (target - pos);
  CPoint dir = normDir(delta);

  CPoint p1 = (pos + dir);
  if (canPass(p1)) { return true; }
  CPoint dirV = dir, dirH = dir;
  dirV.x = 0; dirH.y = 0;

  p1 = (pos + dirV);
  if (canPass(p1)) { return true; }

  p1 = (pos + dirH);
  if (canPass(p1)) { return true; }

  return false;
}


bool Mob::chasePlayer(CPoint dir) {
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
  return bLegal;
}



double MonsterMob::actAngry() { // returns time that action requires (0 means keep doing actions/keep initiative.)
  CPoint dir = playerDir();

  if (playerOnStar()) {
    // if (nearPlayer()) { will change prob. (longer range, higher chance of spell attack.)
    int castChance = (nearPlayer() ? 7 : 33);
    bool willCast = rnd::pctChance(castChance);
    bool canCast = canSeePlayer();
    if (willCast && !canCast) {
      { logstr log; log << pronoun() << " sputters: 'You..hiding coward!'"; }
    }

    if (willCast && canCast) {
      bool bOK = mobCasts(dir);
      return 1.0; // good or bad, at least he's spent his turn now.  // if (bOK) { return 1.0; } // At least he used up his turn now..
    }
  }

  if (nearPlayer()) {
    debstr() << "I am near player and will attack!\n"; // JG, If player is on neighbour tile, we should ALWAYS attack.
     mobMeleesPlayer(dir);    
  } else { // Else, chase the player:
    chasePlayer(dir);
  }

  if (lowHealth()) {
    if (rnd::oneIn(3)) {
      // monster 
      logstr log; log << pronoun() << " flees, feeling hurt.";
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

CPoint Mob::normDir(CPoint delta) {
  CPoint dir(0,0);
  if (delta.x > 0) { dir.x = 1; }
  if (delta.x < 0) { dir.x = -1; }
  if (delta.y > 0) { dir.y = 1; }
  if (delta.y < 0) { dir.y = -1; }
  return dir;
}

CPoint Mob::playerDir() const {
  CPoint delta = (PlayerMob::ply->pos - pos);
  CPoint dir = normDir(delta);
  return dir;
}


void Mob::moveM(CPoint newpos) { // 'does all', 'move mob on map'.
  CL->map.moveMob(*this, newpos);
}



void Mob::makeAngry() {
  if (mood == M_Sleeping || mood == M_Wandering) {
    mood = M_Angry;
    logstr log; log << "You have made " << this->the_mob() << " angry.";
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
    int dx = rnd::rndC(-1, 1), dy = rnd::rndC(-1, 1);
    walk.newpos = (pos + CPoint(dx, dy));
    if (!walk.legal(ss)) {

      CPoint dirV = dir, dirH = dir;
      dirV.x = 0; dirH.y = 0;
      walk.newpos = (pos + dirV); // what about going vertical?
      if (!walk.legal(ss)) { // if vertical doesn't work, what about horizontal?
        walk.newpos = (pos + dirH);
        if (!walk.legal(ss)) { // if horizontal doesn't work, what about fighting back..
          if (nearPlayer()) {
            logstr log; log << "Cornered, the scared " << this->pronoun() << " fights back!"; // fixme, we lack indef_mob.
            const bool dontOverrideHit = false;
            HitCmd mobCornered(NULL, *this, dir.x, dir.y, SC_Phys, SP_NoSpell, dontOverrideHit);
            mobCornered.Do(ss);
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

double MonsterMob::actGeneric() {
  return 1.0;
}

/* ideas - rnd in namespace
better dmg understand for rush/crush etc.
manual control of light strength, to determine sweet-spot.
ai details
DONE: los demand for drawing mobs.
*/


bool MonsterMob::hurt() { 
  const MobDef& def = mobDef();  
  // bool bLow = ( (stats.hp/def.moralePct) < (stats.maxHP/100) ); 
  bool bLow =    ( (stats.hp*100) < (stats.maxHP*def.moralePct) ); 
  return bLow;
}

bool MonsterMob::can_flee() { 
  return canFlee(PlayerMob::ply->pos); 
}

bool MonsterMob::flee_prob() { 
  const MobDef& def = mobDef();  
  return rnd::pctChance(def.retreatPct);  
}

bool MonsterMob::flee() { 
  actFlee();
  return true;
}

bool MonsterMob::can_attack() { 
  // Hmm, did I interpret 'can-attack' correctly, or did it just mean 'can-melee'..?)
  if (melee_range()) {
    return true;
  } else {
    return can_ranged();
  }
}

bool MonsterMob::hurt_attack_prob() { 
  const MobDef& def = mobDef();  
  return rnd::pctChance(def.chargePct); 
}

bool MonsterMob::attack() { 
  bool bOK = false;
  if (melee_range()) {
    bOK = attack_melee();
  } else {
    bOK = attack_ranged();
  }
  return bOK; 
}

bool MonsterMob::too_close() { 
  const MobDef& def = mobDef();  
  int dist = PlayerMob::distPlyCart(pos); // not distPly, so far.
  return (dist < def.minrange); 
}

bool MonsterMob::can_incr() { 
  return canFlee(PlayerMob::ply->pos); 
}

bool MonsterMob::incr_prob() {
  const MobDef& def = mobDef();  
  return rnd::pctChance(def.retreatPct);  
  return false; 
}

bool MonsterMob::incr_dist() { 
  actFlee();
  return true; // possibly we should track if we succeeded or not (?)
}

bool MonsterMob::too_far() { 
  const MobDef& def = mobDef();  
  int dist = PlayerMob::distPlyCart(pos); // not distPly so far.
  return (dist > def.maxrange); 
}

bool MonsterMob::can_decr() { 
  return canChase(PlayerMob::ply->pos); 
}

bool MonsterMob::decr_prob() { 
  const MobDef& def = mobDef();  
  return rnd::pctChance(def.chargePct);  
}

bool MonsterMob::decr_dist() { 
  CPoint dir = playerDir();
  return chasePlayer(dir);
}

bool MonsterMob::melee_range() { 
  return nearPlayer();
}

bool MonsterMob::melee_prob() { 
  const MobDef& def = mobDef();  
  return rnd::pctChance(def.chargePct);  
}

bool MonsterMob::attack_melee() { 
  CPoint dir = playerDir();
  return mobMeleesPlayer(dir);    
}

bool MonsterMob::can_ranged() { 
  return playerOnStar() && canSeePlayer();
}

bool MonsterMob::ranged_prob() { 
  const MobDef& def = mobDef();  
  return rnd::oneIn(2); 
}

bool MonsterMob::attack_ranged() { 
  CPoint dir = playerDir();
  return mobCasts(dir);
}

// stare, gaze
bool MonsterMob::stay() { return true; } // possibly this should cast some spell.


double MonsterMob::actDriven() {
  /* todo - compare with article http://www.roguebasin.com/index.php?title=Roguelike_Intelligence_-_Stateless_AIs
  to understand if I'm missing out on features he manages to fit in.
  */
  if (hurt()) { // (morale-check)
    if (can_flee() && flee_prob()) { flee(); return 1.0; }
    if (can_attack() && hurt_attack_prob()) { attack(); return 1.0; }
    stay(); // Stay may involve spellcast/healing/alerting.
  } else { // Not hurt.
    if (too_close() && can_incr() && incr_prob() ) { incr_dist(); return 1.0; }
    if (too_far() && can_decr() && decr_prob() ) { decr_dist(); return 1.0; }
    if (melee_range() & melee_prob()) { attack_melee(); return 1.0; }
    if (can_ranged() & ranged_prob()) { attack_ranged(); return 1.0; }
    stay();
  }
  return 1.0;  // shouldn't ever reach this point..
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
  } else {
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



