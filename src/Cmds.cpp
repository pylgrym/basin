#include "stdafx.h"
#include "Cmds.h"

#include "LogEvents.h"

#include "MobQueue.h"

#include <fstream>

/* Todos: 
 - "DONE" a dashboard on the right or bottom, with stats (or just a command to print them?)
   (Q will show current stats.)

 - various dmg-spells, should actually do some damage!

 - effects from armour should have an effect
 - armour should give armour class..
*/

bool ShowEventsCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "showing event-log command.\n";
  LogEvents::log.show();
  return true; 
}




bool WalkCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  CL->map.moveMob(mob, newpos);
  if (mob.isPlayer()) { mob.lightWalls(mob.pos); } // ; } // ctype() == CR_Player

  if (mob.isPlayer() && CL->map[newpos].item.o != NULL) {  // ctype() == CR_Player
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

  std::stringstream dummy;
  bool isPlayer = mob.isPlayer();  

  AttackInf ai;
  bool bHit = mob.calcAttack(hitItem, *hittee, ai, school, isPlayer ? err : dummy); 
  if (!bHit) { 
    if (mob.isPlayer()) {  
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
    if (isPlayer) {
      PlayerMob::ply->stats.gainKillXP(hittee->stats.level());
    }

    bool bLoot = oneIn(2);
    if (bLoot) {
      CL->map.addObjAtPos(hittee->pos,CL->level);
      if (oneIn(8)) {
        CL->map.scatterObjsAtPos(hittee->pos, rnd(1,6),CL->level);
      }
    }
    CL->mobs.deleteMob(hittee);
    logstr log; log << "It died.";
    if (bLoot) {
      log << "An item rolls on the floor.";
    }
  }
  return true;
}






bool ZapCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  bool bFound = false;
  int dirKey = 0;

  // FIXME - respectMultiNotif and promptForKey should be integrated!
  LogEvents::respectMultiNotif(); // Pause if we have queued messages, before prompting.
  Cuss::clear(false); // Must come after respectMultiNotif, or we'll never see msg.
  const char* keyPrompt = "Zap which direction?";
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

  CPoint tileMagic(18, 24);
  CPoint tileFire(22, 24);
  CPoint tileFrost(23, 24);
  CPoint tilePurple(34, 24);
  CPoint tileYellow(35, 24);
  CPoint tileGreenFire(39, 24);
  CPoint tileGreenBall(3, 25);
  CPoint tileLight(39, 2);
  CPoint tileNether(0, 3);
  CPoint tileEarth(16, 22);
  CPoint tileStar(15,22);
  CPoint tileWeird(39, 2);
  CPoint tileNone(0, 0); // Yeah it's an ant, but it's also 'none', in this context..

  CPoint tile = tileMagic;
  switch (school) {
  case SC_Magic: tile = tileMagic; break;
  case SC_Fire:  tile = tileFire; break;
  case SC_Frost: tile = tileFrost; break;
  case SC_Earth: tile = tileYellow; break;
  case SC_Gas:   tile = tileGreenBall; break;
  case SC_Light: tile = tileLight; break;
  default:       tile = tileWeird; break;
  }

  CPoint dir = Map::key2dir(dirKey); 
  CPoint bullet = tgt;
  const int maxBulletRange = 50;
  for (int shoot = 0; shoot < maxBulletRange; ++shoot) {
    // Clear old bullet pos:
    CL->map[bullet].overlay = tileNone; // CPoint(0, 0);
    CPoint oldBullet = bullet;

    // Consider new bullet pos:
    CPoint newBullet = bullet;
    newBullet += dir;
    if (!CL->map.legalPos(newBullet)) { break; }

    if (!CL->map[newBullet].creature.empty()) { // We hit a mob..
      CPoint aim = newBullet - tgt;
      { logstr log; log << "The bullet hits the monster."; } 
      // FIXME - attackschool instead of 'bullet', e.g. 'firebolt/ball'
      // FIXME - items must hit much harder. HitCmd should pass the weapon along I think,
      // this way it can both use item and weapon..
      HitCmd cmd(zapHitItem, mob, aim.x, aim.y, school);
      bool bOK = cmd.Do(err);
      break;
    }

    if (CL->map[newBullet].blocked()) { // Did we hit some wall instead?
      if (effect == SP_StoneToMud) { 
        CL->map[newBullet].envir.setType(EN_Floor);
        mob.invalidateGfx(newBullet, oldBullet, true);
        logstr log; log << "The wall turns to mud!";
        TheUI::microSleepForRedraw(7);
      }
      break;
    }

    // It's legal, move to it:
    bullet = newBullet;
    CL->map[bullet].overlay = tile; // CPoint(23, 24); // c = '*';
    mob.invalidateGfx(bullet, oldBullet, true);

    if (effect == SP_WallBuilding) {
      CL->map[bullet].envir.setType(EN_Wall);
    }
    if (effect == SP_LightDir) {
      CL->map[bullet].lightCells();
    }

    TheUI::microSleepForRedraw(7); // 4);
  }

  CL->map[bullet].overlay = tileNone; // Clear the 'last' bullet.
  mob.invalidateGfx(bullet, bullet, true);

  if (effect == SP_WallBuilding) {
    logstr log; log << "Solid rock forms out of thin air!";
  }
  if (effect == SP_LightDir) {
    logstr log; log << "A beam of shimmering light appears!";
  }

  Cuss::clear(false);// true); // Are we sure, last bullet is cleaned up?
  return true; 
}





bool DigCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  Envir& envir = CL->map[tgt].envir;

  int digStr = mob.digStrength();
  envir.envUnits -= digStr;
  if (envir.envUnits > 0) {
    logstr log; log << "You dig in the wall. " << digStr << "/" << envir.envUnits;
    return true;
  }

  envir.setType(EN_Floor);
  mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on CL->map/Cell! (maybe)
  {
    logstr log; log << "You'ved dug through the wall!";
  }
  if (oneIn(3)) { // Loot?
    logstr log; log << "You found something embedded in the rock!";
    CL->map.addObjAtPos(tgt,CL->level);
    if (oneIn(8)) {
      CL->map.scatterObjsAtPos(tgt, rnd(1,6),CL->level);
    }
    mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on CL->map/Cell! (maybe)
  }

  debstr() << "Digged through the wall.\n";
  return true; 
}



bool StairCmd::Do(std::ostream& err) {
  if (!Cmd::Do(err)) { return false; }

  debstr() << "walking stairs.\n";
  int dir = 0;
  if (etype == EN_StairDown) { dir = 1; }
  if (etype == EN_StairUp) { dir = -1; }
  PlayerMob* ply = PlayerMob::ply;
  int newDungLevel = ply->dungLevel + dir;
  if (newDungLevel < 0) {
    logstr log; log << "The stairs collapse as you try to ascend them.";
    return false;
  }
  ply->dungLevel = newDungLevel;
  Dungeons::setCurLevel(newDungLevel);

  /* fixme/consider: player pos should actually be matched to some stairs in this other level!
  in particular, we risk putting him in the middle of rock! 
  */
  EnvirEnum oppo = (dir > 0 ? EN_StairUp : EN_StairDown);
  CPoint newPos = CL->map.findNextEnvir(ply->pos, oppo);
  if (newPos.x >= 0) {
    CL->map.moveMob(*ply, newPos);
  }

  logstr log; log << "You ";
  if (dir > 0) { log << "descend"; } else  { log << "ascend"; }
  log << " to depth level " << newDungLevel;

  Cuss::clear(true);
  return true;
}



bool SaveCmd::Do(std::ostream& err) {
  const char* file = "basin.sav";
  std::ofstream os(file);
  Persist p(os);
  //status87
  bool bSaveOK = CL->persist(p);
  err << "You saved:" << bSaveOK;
  return bSaveOK;
  /*fixme/todo: instead of 'save' and ostream,
  change it to 'persist', called with a persistObj,
  which includes a 'direction' (save or load), 
  and add something that allows us to persist integers (and doubles?),
  with something like '
  persistObj.transfer(& item, "label");
  on top, we need something like handling lists with lengths, possibly specifying the length at front.
  also
  */
}


bool LoadCmd::Do(std::ostream& err) {
  const char* file = "basin.sav";
  std::ifstream is(file);
  Persist p(is);
  //status87
  bool bLoadOK = CL->persist(p);
  err << "You loaded:" << bLoadOK;
  return bLoadOK;
  /*fixme/todo: instead of 'save' and ostream,
  change it to 'persist', called with a persistObj,
  which includes a 'direction' (save or load),
  and add something that allows us to persist integers (and doubles?),
  with something like '
  persistObj.transfer(& item, "label");
  on top, we need something like handling lists with lengths, possibly specifying the length at front.
  also
  */
}
