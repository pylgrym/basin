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
  if (mob.ctype() == CR_Player) { mob.lightWalls(mob.pos); }

  if (mob.ctype() == CR_Player && Map::map[newpos].item.o != NULL) { 
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
  AttackInf ai;
  bool bHit = mob.calcAttack(*hittee, ai); // dmg);
  if (!bHit) { 
    if (mob.ctype() == CR_Player) {
      err << "You miss! "; 
    } else {
      err << "It misses. "; 
    }
    ai.rep(err, mob.stats);
    return false; 
  }
  // --hittee->stats.hp;

  {
    logstr log;
    log
      << mob.pronoun()
      << " hit" << mob.verbS() << " "
      << hittee->pronoun() << " for " << ai.dmgTaken << ".";

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
