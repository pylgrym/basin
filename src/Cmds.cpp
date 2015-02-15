#include "stdafx.h"
#include "Cmds.h"

#include "LogEvents.h"

bool ShowEventsCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "showing event-log command.\n";
  LogEvents::log.show();
  return true; 
}




bool WalkCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  Map::map.moveMob(mob, newpos);
  if (mob.isPlayer()) { mob.lightWalls(mob.pos); } // ; } // ctype() == CR_Player

  if (mob.isPlayer() && Map::map[newpos].item.o != NULL) {  // ctype() == CR_Player
    LookCmd look(mob);
    look.Do(err);
  }

  mob.invalidateGfx(newpos, old, false);
  TheUI::invalidateWndJG(NULL, false);
  return true;
}


/* I could make some spells..
two teleport spells - short range, and long range.
 - wall-building/modifying spells.
I have no mana. I have no stats.
I have no armour.
I don't have an active weapon.
I don't have any equipped gear - no rings, no weapons, no armour.
  So, it would make sense to design an 'equip slots' interface
 - a slots-menu of head,neck,shoulders,chest,wrists,hands,legs,belt,boots, 
 mainhand-weapon, off-hand weapon/shield.
 - as part of inventory, a second 'restricted' inventory
 (only one 'swappable' item per slot, 'left ring/right ring'.)

 However, for now I need more fun 'fighting/interaction mechanics'
  - all those spells and items make little sense, if there is no challenging combat.
The monsters are very few, they have zerolike AI, both in movement, attacks and general behaviour.

I DO have hitpoints though.
I don't have any way to re-gain my hit points.
I might get hitpoints from 
- eating
- healing
- waiting. DONE!

 - some uses, might ask confirmation - "are you sure you want to eat/drink this unidentified x?"

 monsters that multiply.. hmm, hate them.

 monsters that require specific items to counter, so I need to get to the 
 counter-items to handle them.
   Modes my player can be in, that makes him weak to some, strong against others..
 E.g. a fire-element-mode, where I'm resistant to fire, but vulnerable to water/frost.
 Possibly a stone-paper-scissors for earth+fire+air+water?
  - water puts out fire. fire burns.. air?
  - air is strong against earth?
  - fire is strong against earth?
  - air is weak to fire, 
  fire is strong to 

  water > fire, 
  fire  > air
  air   > earth
  earth > water
  W->F->A->E
   - You can't make a second set in even numbers, because jumping 2, you go from 4 to 2 frequency.

   make light.
   29-20 black, 29-21 water
*/







bool HitCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  hittee->invalidateGfx(tgt, tgt, true);

  // int dmg = 0;
  std::stringstream dummy;
  bool isPlayer = mob.isPlayer(); // ctype() == CR_Player;

  AttackInf ai;
  bool bHit = mob.calcAttack(*hittee, ai, isPlayer ? err : dummy); // dmg);
  if (!bHit) { 
    if (mob.isPlayer()) { // mob.ctype() == CR_Player) {
      err << "You miss! "; 
    } else {
      err << "It misses. "; 
    }

    err << "(";
    ai.repHitChance(err);
    err << "%)";

    ai.rep(err, mob.stats);
    return false; 
  }
  // --hittee->stats.hp;

  {
    logstr log;
    log
      << mob.pronoun()
      << " hit" << mob.verbS() << " "
      << hittee->pronoun() << " for " << ai.dmgTaken;

    log << "(";
    ai.repHitChance(log);
    log << "%)";

  }
 
  {
    logstr log;
    ai.rep(log, mob.stats);
    log << " R.HP:" << hittee->stats.hp; // Remaining 
  }

  // mob should die if killed:
  if (hittee->isDead()) {
    MobQueue::mobs.deleteMob(hittee);
    logstr log; log << "It died.";
  }
  return true;
}




CPoint Map::key2dir(char nChar) {
  int dx = 0, dy = 0;
  // determine movement:
  switch (nChar) { case VK_RIGHT:  case 'L': case 'U': case 'N': dx = 1;  }
  switch (nChar) { case VK_LEFT:   case 'H': case 'Y': case 'B': dx = -1; } 
  switch (nChar) { case VK_DOWN:   case 'J': case 'B': case 'N': dy = 1;  } 
  switch (nChar) { case VK_UP:     case 'K': case 'Y': case 'U': dy = -1; } 
  CPoint dir(dx, dy);
  return dir;
}


bool ZapCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  const char* keyPrompt = "Zap which direction?";
  bool bFound = false;
  int dirKey = 0;
  for (;!bFound;) {
    dirKey = TheUI::promptForKey(keyPrompt); 

    if (dirKey == VK_ESCAPE) {
      Cuss::clear(true);
      return false; // Cancelled zap operation.
    }
    
    switch (dirKey) {
    case 'H': case 'J': case 'K': case 'L': case 'N': case 'B': case 'U': case 'Y': bFound = true;  break;
    default: TheUI::BeepWarn(); break; // Not a DIR key.
    }      
  } // Loop until dir key.

  CPoint dir = Map::key2dir(dirKey); // (1, 0); // FIXME; should follow key.
  CPoint bullet = tgt;
  for (int shoot = 0; shoot < 10; ++shoot) {
    // Clear old bullet pos:
    Map::map[bullet].c = 0;
    CPoint oldBullet = bullet;

    // Consider new bullet pos:
    CPoint newBullet = bullet;
    newBullet += dir;
    if (!Map::map.legalPos(newBullet)) { break;  }
    if (Map::map[newBullet].blocked()) {
      break;
    }
    // It's legal, move to it:
    bullet = newBullet;
    Map::map[bullet].c = '*';
    mob.invalidateGfx(bullet, oldBullet, true);
    TheUI::microSleepForRedraw(); 
  }

  Cuss::clear(true);
  return true; 
}
