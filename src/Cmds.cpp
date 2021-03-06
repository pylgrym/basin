#include "stdafx.h"
#include "Cmds.h"
#include "LogEvents.h"
#include "MobQueue.h"
#include <fstream>
#include "util/debstr.h"
#include "ShopInv.h"
#include "PlayerMob.h"

#include "stdlib.h" // abs.

#include "FightDashboard.h"


#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")

/* Todos: 
 - "DONE" a dashboard on the right or bottom, with stats (or just a command to print them?)
   (Q will show current stats.)

 - various dmg-spells, should actually do some damage!

 - effects from armour should have an effect
 - armour should give armour class..
*/

void playSound(CString soundFile) {
  PlaySound(soundFile, NULL, SND_FILENAME | SND_ASYNC); 
}


bool ShowEventsCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "showing event-log command.\n";
  LogEvents::log.show();
  return true; 
}



bool WalkCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  CL->map.moveMob(mob, newpos);
  if (mob.isPlayer()) { mob.lightWalls(); } 

  if (mob.isPlayer()) {
    // disabling constant 'bell' sound, for now..:
    //playSound(L"sounds\\chosen\\walk3.wav"); // walk@.wav"); // was: walk2.wav

    if (mob.isPlayer() && CL->map[newpos].envir.interacts()) {
      ShopCmd shop;
      bool bOK = shop.Do(err);
    } else { // Don't look if it was a shop.
      if (CL->map[newpos].item.o != NULL) {
        LookCmd look(mob);
        look.Do(err);
      }
    }

  }


  mob.invalidateGfx(newpos, old, false);
  // TheUI::invalidateWndJG(NULL, false);
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





/* todo - make viewport go 2 rows down. */

bool HitCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  hittee->invalidateGfx(tgt, tgt, true);

  std::stringstream dummy;
  bool isPlayer = mob.isPlayer();  

  AttackInf ai;
  bool bHit = mob.calcAttack(isPlayer ? err : dummy, ai, *hittee, hitItem, school, spell, overrideHit); // in HitCmd::Do.

  if (mob.isPlayer()) {
    FightDashboard::dashboard.hp = hittee->stats.hp;
    FightDashboard::dashboard.maxHP = hittee->stats.maxHP;
    FightDashboard::dashboard.lastHit = ai.dmg;
  }

  if (!bHit) { 
    if (mob.isPlayer()) {  
      playSound(L"sounds\\sfxr\\failure2.wav"); // HIT MOB
      err << "You miss! "; 
    } else {
      err << mob.pronoun() << " misses. "; 
    }

    err << "(th";
    ai.repHitChance(err);
    err << "%)";

    ai.repToHitCheck(err, mob.stats);
    return true; // even if you miss, you've used up your turn. 
  } else { // It HITS!
    if (mob.isPlayer()) {
      playSound(L"sounds\\sfxr\\walk2.wav"); // HIT MOB
    }

    {
      logstr log; // First, show if roll succeeded or not (to-hit check):
      ai.repToHitCheck(log, mob.stats);
      log << " R.HP:" << hittee->stats.hp; // Remaining 
    }

    { // Second, show details of the attack roll:
      logstr log; log << ai.dmgRollInfo;
    }

    logstr log; log // finally, show the conclusion of the attack:
      << mob.pronoun() << mob.stats.level() // "you hit"<-grep-target-string..
      << " hit" << mob.verbS()  << " "
      << hittee->pronoun() << " for " << ai.dmgTaken;

    log << "(";
    ai.repHitChance(log);
    log << "%)";

  }
 

  // mob should die, if killed:
  if (hittee->isDead()) {

    /* the order is a bit tricky here..
    we need to order:
     - A mob death-announcement.
     - B item-drops
     - C possible xp+level gain
     - D dead-mob-cleanup
    I definitely want A before C,
    but I have to do C before D (because I need the mob-info to do it.)
      Also, I'd prefer to do D before B.
    I could resolve some of it, by caching (e.g. cache deadmob-info for C,
    to be able to do D before B.)
    */

    if (hittee->isPlayer()) {
      logstr log; log << "Alas! You died!..";
    } else {
      logstr log; log << hittee->the_mob() << " died.";
    }

    bool bLoot = rnd::oneIn(2);
    if (bLoot) {
      CL->map.addObjAtPos(hittee->pos, CL->level);
      if (rnd::oneIn(8)) {
        CL->map.scatterObjsAtPos(hittee->pos, rnd::Rnd(1,6), CL->level, 1);
      }

      playSound(L"sounds\\sfxr\\pickaxe1@.wav"); // loot drops
      logstr log; log << "An item rolls on the floor.";
    }

    if (isPlayer) {
      PlayerMob::ply->stats.gainKillXP(hittee->stats.level());
    }

    CL->mobs.deleteMob(hittee); // in HitCmd::Do.
  }
  return true;
}






int crossDistance(CPoint a, CPoint b) { // Returns the longer of the 2 axis-distances.
  // FIXME, move to a util-lib.
  CPoint d = a - b;
  d.x = abs(d.x); d.y = abs(d.y);
  return (d.x > d.y ? d.x : d.y);
}

/* design confusion: 
hitcmd, zapcmd, dospell and bulletspell are too mixed-up in each other
 - the mechanisms they handle: mob damage, projectile spells etc., 
should go into a coherent design.
*/


bool ZapCmd::legal(std::ostream& err) {
  if (!Cmd::legal(err)) { return false;  }

  if (mob.stats.isBlind()) {
      err << mob.pronoun() << " can't see anything!";
      return false;
  }
  
  return true;

  // Consider - prompt the user, if he wants to risk it.
  // if (!consumeMana) { return true; }
  //bool manaCheckOK = Spell::manaCostCheck(effect, mob, err);
  //return manaCheckOK;
}


bool ZapCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }
  /* FIXME, none of this is really a Cmd - this is a utility helper for Spell class!
  (used by bulletspell mainly, and secondly by monstermob.) */
  CPoint dir = mobZapDir;
  // if (mob.isPlayer()) { // Player chooses dir interactively:
  //  dir = Spell::pickZapDir();
  //  if (dir == Spell::NoDir) { return false; }
  //}

  CPoint tile = Spell::bulletTileForSchool(school);
  CPoint tileNone(0, 0); // Yeah it's an ant, but it's also 'none', in this context..
  const SpellDesc& spellDesc = Spell::spell(effect); // used by range-check.

  playSound(L"sounds\\sfxr\\sweep.wav"); // travel-zap-bullet . split.wav chirp4.wav +frostbalt.wav
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

    if (crossDistance(newBullet, tgt) > spellDesc.maxRange) {
      logstr log; log << "The spell sputters and dies away, in the distance.";
      break;
    }

    // It's legal, move to it:
    bullet = newBullet;
    CL->map[bullet].overlay = tile;
    mob.invalidateGfx(bullet, oldBullet, true);


    /* fixme - the zapcmd-bullet-loop,
    should be turned into an iterator-visitor-like thingy.
     it should only handle the movement of projectiles/graphics.
    if it's structured like an iterator, it would allow you to 
    'iterate' the bullet-travel, until you encounter distinct stuff 
     - an enemy, or a wall.
     (or, alternatively, while you are passing empty squares to light or wall-build.)
    I might use the new 'slide-player' spell to prototype this.
    */

    extern bool extTeleportTo(Mob& actor, CPoint targetpos, bool announce); // , Mob* aim);

    if (!CL->map[newBullet].creature.empty()) { // We've hit a mob..
      /* fixme, design: spell's minrange and maxrange should be honoured,
      and have an effect - beware they should be 'safe/secure',
      and not just allow exploiting, e.g. shooting through walls
      (ie if you have a minimum range, the bullet must still travel before that.
      */
      if (crossDistance(newBullet, tgt) < spellDesc.minRange) {
        logstr log; log << "The spell sizzles out, in too short range!";
        // clean-up gfx layer!
        CL->map[newBullet].overlay = tileNone; 
        break; 
      }
      Mob* target = CL->map[newBullet].creature.m;
      { logstr log; log << "The " << Spell::bulletTxt(effect) << " hits " << target->pronoun() << "."; } // monster."; } 

      CPoint aim = newBullet - tgt;

      switch (effect) {
      case SP_Speedup: case SP_Slowdown: case SP_ConfuseSelf: case SP_Unconfuse: case SP_TeleOtherAway: case SP_TeleSelfAway: //  - No - NO SP_ConfuseMob here! (because it's a bullet spell.)
      case SP_Heal_light: case SP_Heal_minor: case SP_Heal_mod: case SP_Heal_serious: case SP_Heal_crit: case SP_Sick:
        { logstr log;
          bool bSpellOK = Spell::castSpell(effect, *target, NULL, zapHitItem, NoMana); // in zapcmd. hitting a mob.
          if (bSpellOK && mob.isPlayer()) { Spell::trySpellIdent(effect); }
          break;
        }

      case SP_TeleportTo: // mob -> target, dir
        { { logstr log; log << mob.pronoun() << " phase" << mob.verbS() << " next to " << target->pronoun(); }
          CPoint targetpos = target->pos - dir; // The space in front of target.
          bool bSpellOK = extTeleportTo(mob, targetpos, true); // FIXME, integrate with 'exec2' mechanism.
          if (bSpellOK && mob.isPlayer()) { Spell::trySpellIdent(effect); }
          break;
        }

      case SP_TeleSwap: 
        { { logstr log; log << mob.pronoun() << " switch" << mob.verbS() << " place with " << target->pronoun(); }

          extern bool teleportSwap(Mob& actor, Mob& target, bool announce);
          bool bSpellOK = teleportSwap(mob, *target, true);

          // goblin warrior has wrong graphics

          /*
          // it's tricky, because we want each other's space..
          CPoint actorNewpos = target->pos; 
          CPoint targetNewpos = mob.pos;

          // this didn't work correctly?
          //CL->map.clearMob(mob); // ie actor - remove ourselves from map.
          CL->map.setMobForce(mob, actorNewpos, true);
          CL->map.setMobForce(*target, targetNewpos, true);
          //bool bSpellOK = teleportTo(*target, targetNewpos,false); // move 'victim' to our space.
          bSpellOK = teleportTo(mob, actorNewpos, true);
          */

          if (bSpellOK && mob.isPlayer()) { Spell::trySpellIdent(effect); }
          break;
        }

      case SP_SummonHere: // mob <- target, dir
        // weird - i got an 'x pulls the x nearer' from a mob, suggesting it cast the spell on itself?
        { { logstr log; log << mob.pronoun() << " pull" << mob.verbS() << target->pronoun() << " nearer."; }
          CPoint targetpos = mob.pos + dir; // The space just in front of me.
          bool bSpellOK = extTeleportTo(*target, targetpos, true); // FIXME, integrate with 'exec2' mechanism.
          if (bSpellOK && mob.isPlayer()) { Spell::trySpellIdent(effect); } 
          break;
        }

      case SP_ConfuseMob: // This used to be a major bug - confusemob would recurse infinitely, from  dospell, zapcmd, bulletspell loop.
        { logstr log; // (Actually, confusemob actually should just 'bulletspell->confuseSelf')
          bool bSpellOK = Spell::castSpell(SP_ConfuseSelf, *target, NULL, zapHitItem, NoMana); // in zapcmd. hitting a mob.
          // (JG: The "bullet->self" are the reason tryident happens outside, and not directly in castspell.)
          if (bSpellOK && mob.isPlayer()) { Spell::trySpellIdent(effect); } // we identify 'confusemob', not 'confuse'.
          break;
        }

      case SP_SleepOther: 
        { // logstr log;
          extern bool sleepMob(Mob* target);
          bool bSpellOK = sleepMob(target); 
          if (bSpellOK && mob.isPlayer()) { Spell::trySpellIdent(effect); } 
          break;
        }


      default: // ALL BULLET spells, e.g. SP_MagicMissile. I considered even SP_ConfuseMob, but we don't need hitcmd for that.
        {
          // FIXME - attackschool instead of 'bullet', e.g. 'firebolt/ball'
          // FIXME - items must hit much harder. HitCmd should pass the weapon along I think,
          // this way it can both use item and weapon..
          playSound(L"sounds\\sfxr\\firebolt@.wav"); // zap-spell-projectile
          const bool overrideHit = true; // false; // FIXME: also mobs hit this hard now! We need a second, or balanced, to-hit check..
          HitCmd bulletSpellHit(zapHitItem, mob, aim.x, aim.y, school, effect,overrideHit); // Doing a zap.
          bool bOK = bulletSpellHit.Do(err);
        }
      } // (End switch (which-spell-type-hit-mob-target).)
      break; // (Skip out of bullet-travel-loop.)
    }

    if (CL->map[newBullet].blocked()) { // Did we hit some wall instead?
      if (effect == SP_StoneToMud) { 
        CL->map[newBullet].envir.setType(EN_Floor);
        mob.invalidateGfx(newBullet, oldBullet, true);
        playSound(L"sounds\\sfxr\\explodeDeep@.wav"); // stone-to-mud
        logstr log; log << "The wall turns to mud!";
        TheUI::microSleepForRedraw(7);
      }
      break;
    }

    // WAS: redraw was here..

    if (effect == SP_WallBuilding) {
      CL->map[bullet].envir.setType(EN_Wall);
    }
    if (effect == SP_LightDir) {
      CL->map[bullet].lightCells(bullet);
    }

    TheUI::microSleepForRedraw(7); // 4);
  }

  CL->map[bullet].overlay = tileNone; // Clear the 'last' bullet.
  mob.invalidateGfx(bullet, bullet, true);

  if (effect == SP_WallBuilding) {
    playSound(L"sounds\\sfxr\\thunder.wav"); // wall-building
    logstr log; log << "Solid rock forms out of thin air!";
  }
  if (effect == SP_LightDir) {
    playSound(L"sounds\\sfxr\\zap2.wav"); // light-beam // krlorrk.wav
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
    playSound(L"sounds\\sfxr\\anvil1@.wav"); // digging
    logstr log; log << "You dig in the wall. " << digStr << "/" << envir.envUnits;
    return true;
  }

  envir.setType(EN_Floor);
  mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on CL->map/Cell! (maybe)
  {
    logstr log; log << "You've dug through the wall!";
  }
  if (rnd::oneIn(3)) { // Loot?
    logstr log; log << "You found something embedded in the rock!";
    CL->map.addObjAtPos(tgt,CL->level);
    if (rnd::oneIn(8)) {
      CL->map.scatterObjsAtPos(tgt, rnd::Rnd(1,6),CL->level,1);
    }
    mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on CL->map/Cell! (maybe)
  }

  debstr() << "Digged through the wall.\n"; // I know it's 'dug'.
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

  playSound(L"sounds\\chirp4.wav"); // go stairs
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
  bool bSaveOK = Dungeons::the_dungeons.persist(p);

  err << "You saved:" << bSaveOK;
  return bSaveOK;
}


bool LoadCmd::Do(std::ostream& err) {
  const char* file = "basin.sav";
  std::ifstream is(file);
  Persist p(is);
  bool bLoadOK = Dungeons::the_dungeons.persist(p);

  err << "You loaded:" << bLoadOK;
  return bLoadOK;
}




bool BuyCmd::Do(std::ostream& err) {
  Obj* buy = ShopInv::shop.pickAction();
  if (buy == NULL) {
    return false;
  }
  if (buy->price() > PlayerMob::ply->stats.gold) {
    { pauselog log; log << "It costs too much for you to buy.";  }
    return false;
  }

  PlayerMob::ply->stats.gold -= buy->price();
  ShopInv::shop.remove(buy, err);
  Bag::bag.add(buy,err);
  playSound(L"sounds\\split.wav"); // buying
  { pauselog log; log << "You buy it for " << buy->price() << " gold."; }

  return true;
}


bool ShopCmd::Do(std::ostream& err) {

  // NB! we use 'respectMultiNotif' to '-more'-pause after our actions,
  // because we want to re-display the menu afterwards (pauselog handles this automatically.)

  bool mustClear = true;
  for (; ;) { 
    std::stringstream ss; 
    ss << "B to Buy, S to Sell.";
    ss << " You have " << PlayerMob::ply->stats.gold << " gold."; // const char* keyPrompt = 
    std::string keyPrompt = ss.str();

    if (mustClear) {
      Cuss::clear(false);
      Cuss::prtL("  Welcome to my shop, traveller!");
      ShopInv::shop.showShopInv();
      mustClear = false;
    }

    int cmdKey = TheUI::promptForKey(keyPrompt.c_str(), __FILE__, __LINE__, "buy-or-sell-shop-choice");
    switch (cmdKey) {
    case VK_ESCAPE: Cuss::clear(true); return false; // Cancelled shopping.
    case 'B':       { BuyCmd  cmd; cmd.Do(err); mustClear = true; } break;
    case 'S':       { SellCmd cmd; cmd.Do(err); mustClear = true; } break;
    default:        TheUI::BeepWarn(); break; // Not a CMD key.
    }
  } // Loop until command key or escape.

  Cuss::clear(true);
  return true;
}



bool SellCmd::Do(std::ostream& err) {
  Obj* obj = Bag::bag.pickBag("  Sell what?", true);
  if (obj == NULL) { return false; }

  PlayerMob::ply->stats.gold += obj->price();
  Bag::bag.remove(obj, err);
  ShopInv::shop.add(obj, err);
  debstr() << "sold" << (void*) obj << "\n";

  playSound(L"sounds\\sfxr\\coinJingle1.wav"); // selling
  { pauselog log; log << "You sell it for " << obj->price() << " gold."; }

  debstr() << "after notif." << (void*)obj << "\n";

  // NB: Selling should ID objs'! 
  obj->soloident = true; // When in shop, its known.

  return true;
}




bool DropCmd::Do(std::ostream& err) {
  debstr() << "drop-cmd do begin..\n";
  if (!Cmd::Do(err)) {
    return false;
  }

  debstr() << "doing drop item-command.\n";
  Obj* obj = Bag::bag.pickBag("  Drop what?", false);
  if (obj == NULL) { return false; }

  debstr deb;
  if (!Bag::bag.remove(obj, deb)) { deb << "\n";  return false; }

  ObjSlot& item = CL->map[tgt].item;
  item.setObj(obj);
  // (No need to invalidate, as we are standing on it)
  mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on CL->map/Cell! (maybe)

  std::string anItem = obj->an_item();

  playSound(L"sounds\\sfxr\\thump.wav"); // drop item.
  err << "You drop " << anItem;
  return true;
}



bool DexModCmd::Do(std::ostream& err) {
  debstr() << "dexmod-cmd do begin..\n";
  PlayerMob::ply->stats.Dex.base += delta;
  PlayerMob::ply->stats.calcStats();
  Cuss::clear(true);
  return true;
}

bool LightModCmd::Do(std::ostream& err) {
  debstr() << "lightmod-cmd do begin..\n";
  Obj* lamp = PlayerMob::ply->findLight();
  if (lamp == NULL) { return false; }
  double rate = (100.0 + delta) / 100.0;
  lamp->itemUnits = int(rate * lamp->itemUnits + 0.5);

  PlayerMob::ply->updateLight();
  // PlayerMob::ply->stats.Dex.base += delta;
  //PlayerMob::ply->stats.calcStats();
  Cuss::clear(true);
  return true;
}


bool OverrideLightCmd::Do(std::ostream& err) {
  debstr() << "overridelightmod-cmd do begin..\n";
  PlayerMob::ply->overrideLight = (PlayerMob::ply->overrideLight == 0 ? 200 : 0);
  PlayerMob::ply->updateLight();
  Cuss::clear(true);
  return true;
}



bool FillLampCmd::Do(std::ostream& err) {
  logstr log; log << "You fill the lamp.";
  Obj* lamp = PlayerMob::ply->findLight();
  if (lamp == NULL) {
    err << "but you have no lamp?"; return false;
  }
  lamp->itemUnits += oil->itemUnits;
  {
    playSound(L"sounds\\krlorrk.wav"); // fill lamp
    logstr log; log << "You refill your lamp. It burns brighter!";
  }
  Cuss::clear(true); // Redraw, so we can see updated lamp oil stats, and the brighter light.
  return true;
}



/* f8/shift f8,is the 'new' f4 (does it work for
several kinds of lists?) */

bool RollStatsCmd::Do(std::ostream& err) {
  debstr() << "doing rollstats command.\n";

  while (1) {
    // NB, all variations of clear still leaves dungeon drawn below - no full screen clear
    // (use clearLine instead I guess? - how dow inv draw?) prtL will fill full line across screen.
    Cuss::clear(false); 
    Cuss::prtL("Roll your stats:");
    Cuss::prtL(" ");
    Stats tmpStats(1, true);
    tmpStats.showStats();
    Cuss::prtL("");
    int key = 0;
    while (1) {
      const char* prompt = "  (Space to roll, Enter to accept, ESC to stop.)";
      key = TheUI::promptForKey(prompt,__FILE__, __LINE__, "stats-roller");
      if (key == VK_ESCAPE || key == ' ' || key == VK_RETURN) { break; } // VK_ACCEPT 
    } 
    if (key == VK_ESCAPE || key == VK_RETURN) { 
      outStats = tmpStats;
      break;
    }
  }

  Cuss::clear(true);
  return true;
}


bool StatCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "show stats command.\n";

  Cuss::clear(false);
  Cuss::prtL("  Your stats:"); 

  PlayerMob::ply->stats.showStats(); 
  TheUI::promptForAnyKey(__FILE__, __LINE__, "stat-pause");

  Cuss::clear(true);
  return true; 
}



bool TakeCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  ObjSlot& item = CL->map[tgt].item;
  // (No need to invalidate, as we are standing on it)
  mob.invalidateGfx(tgt, tgt, true); // FIXME: invalidateTile should go on CL->map/Cell! (maybe)


  if (Obj::isCurrency(o->otype())) { // Gold is special - it's consumed on pickup, and added to gold balance:
    mob.stats.gold += o->itemUnits;
    std::string idesc = o->indef_item();
    err << "You pick up " << o->itemUnits << " worth of " << idesc; // gold pieces.";
    item.setObj(NULL);
    delete o; 
    return true; 
  } // special-case gold.

  // Everything else non-gold, goes in bag:

  if (!Bag::bag.add(o, err)) { return false; } // err << "Couldn't fit item in bag.";  

  char itemIx = Bag::bag.letterIxStacked(o); // fixme - not good - need stacker to find this!
  // std::string anItem = o->an_item();
  std::string theItem = o->the_item();
  err << "You pick up (" << itemIx << ") " << theItem;
  item.setObj(NULL);
  return true; 
}



bool SpellInvCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "doing spell Inventory command.\n";

  Cuss::clear(false);
  Cuss::prtL("  You know these spells:"); 

  Spell::showSpellInv(0,24);

  TheUI::promptForAnyKey(__FILE__, __LINE__, "inv-pause");

  Cuss::clear(true);
  return true; 
}



bool UseCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "doing use item-command.\n";
  Obj* obj = Bag::bag.pickBag("  Use what?", false);
  if (obj == NULL) { return false;  }

  if (obj->wearable()) {
    return mob.wear(obj, err);
  }

  if (!obj->useObj(mob, err)) {
    return false;
  }
  if (obj->charges == 0 && obj->consumed) {
    err << "You've used up the item.";
    Bag::bag.remove(obj, err);
    delete obj;
  }
  return true; 
}


// DONE: SpellBook, 
// DONE: Persist ID'ed spells. 
// DONE: Mark items as identified when sold to shop.

bool CastCmd::legal(std::ostream& err) {
  if (!Cmd::legal(err)) { return false;  }

  if (actor.stats.isBlind()) {
      err << actor.pronoun() << " can't see anything!";
      return false;
  }

  return true;
}

bool CastCmd::Do(std::ostream& err) {
  if (!Cmd::Do(err)) { return false; }
  debstr() << "doing cast command.\n";

  /* I've introduced 'spellparams'. - it works with multi-switch.
  order is
  1 - choose spell. (pickASpell)
  (the rest 2-5, handled by castSpell) 
  2 - collect complete params.(dir)
  3 - check mana is available or risked.
  4 - consume mana
  5 - execute spell.
  */

  // menu:
  // DONE:PickSpell. 
  SpellEnum spellType = Spell::pickASpell("Cast which spell?"); // (1 - choose spell)
  if (spellType == SP_NoSpell) { return false; }

  bool castOK = Spell::castSpell(spellType, actor, NULL, NULL, UseMana); // in User's cast-cmd.
  return castOK;
}




bool DoorToggleCmd::Do(std::ostream& err) {
  if (!Cmd::Do(err)) { return false; }
  debstr() << "doing open/close door - command.\n";

  // TOdo - ask for direction key.
  bool bFound = false;
  int dirKey = 0;

  // FIXME - respectMultiNotif and promptForKey should be integrated!
  LogEvents::respectMultiNotif(); // Pause if we have queued messages, before prompting.
  Cuss::clear(false); // Must come after respectMultiNotif, or we'll never see msg.
  const char* keyPrompt = "Which door? (dir.)";
  for (;!bFound;) {
    dirKey = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-zap-dir"); 

    if (dirKey == VK_ESCAPE) {
      Cuss::clear(true);
      return false; // Cancelled zap operation.
    }
    
    switch (dirKey) {
    case 'H': case 'J': case 'K': case 'L': case 'N': case 'B': case 'U': case 'Y': bFound = true;  break;
    default: TheUI::BeepWarn(); break; // Not a DIR key.
    }      
  } // Loop until dir key.

  CPoint dir = Map::key2dir(dirKey); 
  CPoint doorPos = mob.pos + dir;
  if (!CL->map.legalPos(doorPos)) { err << "Not a legal position."; return false; }

  Cell& doorCell = CL->map[doorPos];
  if (!doorCell.envir.isDoor()) { err << "That is not a door."; return false; }

  if (doorCell.envir.type == EN_DoorOpen) { doorCell.envir.type = EN_DoorClosed;  }
  else if (doorCell.envir.type == EN_DoorClosed) { doorCell.envir.type = EN_DoorOpen;  }
  switch (doorCell.envir.type) {
  case EN_DoorLocked: { err << "But the door is securely locked.";  return false; }
  case EN_DoorStuck:  { err << "But the door is stuck.";            return false; }
  case EN_DoorBroken: { err << "But the door is broken to pieces."; return false; }
  }
  // DONE: - no drawn tiles for types, and no code that inserts doors!
  // DONE: - handle EN_DoorLocked EN_DoorStuck EN_DoorBroken
  Cuss::clear(true);
  mob.invalidateGfx(doorPos, doorPos, true);

  return false;
}



bool DoorBashCmd::Do(std::ostream& err) {
  if (!Cmd::Do(err)) { return false; }
  debstr() << "doing bash open/close door - command.\n";

  // TOdo - ask for direction key.
  bool bFound = false;
  int dirKey = 0;

  // FIXME - respectMultiNotif and promptForKey should be integrated!
  LogEvents::respectMultiNotif(); // Pause if we have queued messages, before prompting.
  Cuss::clearLine(0,false); // Must come after respectMultiNotif, or we'll never see msg.
  const char* keyPrompt = "Which door? (dir.)";
  for (;!bFound;) {
    dirKey = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-zap-dir"); 

    if (dirKey == VK_ESCAPE) {
      Cuss::clearLine(0,false); // true);
      return false; // Cancelled zap operation.
    }
    
    switch (dirKey) {
    case 'H': case 'J': case 'K': case 'L': case 'N': case 'B': case 'U': case 'Y': bFound = true;  break;
    default: TheUI::BeepWarn(); break; // Not a DIR key.
    }      
  } // Loop until dir key.

  Cuss::clearLine(0,false); // true);

  CPoint dir = Map::key2dir(dirKey); 
  CPoint doorPos = mob.pos + dir;
  if (!CL->map.legalPos(doorPos)) { err << "Not a legal position."; return false; }

  Cell& doorCell = CL->map[doorPos];
  if (!doorCell.envir.isDoor()) { err << "That is not a door."; return false; }

  if (doorCell.envir.type == EN_DoorOpen || doorCell.envir.type == EN_DoorBroken) { err << "But that door is already open!"; return false; }
  if (doorCell.envir.type != EN_DoorLocked && doorCell.envir.type != EN_DoorClosed && doorCell.envir.type != EN_DoorStuck) { err << "That is no door to bash."; return false; }

  // Make a strength break check (use shield?)  
  bool breakDoorSucceed = (mob.stats.Str.rollCheck(true) && mob.stats.Str.rollCheck(true)); // Two checks in a row.
  if (!breakDoorSucceed) {
    err << "The door doesn't budge.";  return true; // we still return true, because we spent the turn!
  }

  { 
    logstr log; log << "The door crashes open!";
  }

  doorCell.envir.type = EN_DoorBroken; // If you bash a door open, it breaks.

  mob.invalidateGfx(doorPos, doorPos, true);

  return false;
}




bool InvCmd::Do(std::ostream& err) {  
  if (!Cmd::Do(err)) { return false; }

  debstr() << "doing bag Inventory command.\n";

  Cuss::clear(false);
  Cuss::prtL("  Your bag contains:"); 

  // Bag::bag.showBagInv(false); // in Inv-command.
  Bag::bag.showBagInvStacked(false);

  TheUI::promptForAnyKey(__FILE__, __LINE__, "inv-pause");

  Cuss::clear(true);
  return true; 
}


bool HelpCmd::Do(std::ostream& err) {
	// if (!Cmd::Do(err)) { return false; }

	debstr() << "doing help command.\n";

	Cuss::clear(false);
	Cuss::prtL("  Keyboard:");

  // idea: lock random seed for dungeon?
  /*
  f8/shift f8,is the 'new' f4 (does it work for
  several kinds of lists?)

  idea: make compact level generators,
  to give each level more personality and difference.
  */

  std::string s[] = {
    "",
    " � 1 2 3 4 5 6 7 8 9 0 +(?)   ",
    "Tab Q W E R T Y U I O P � �   ",
    "     A S D F G H J K L � � '  ",
    "    < Z X C V B N M , . -     ",
    "",
    "Q-stats, W-ear, E-xamine, T-akeoff, I-nv,",
    "O-pen/close,",
    "A-ctivate, S-ee, D-rop, F-bash, G-et,",
    "Z-ap, X-spells, C-stairs.",
    "",
    "@"
  };

  for (int i = 0; i < 100; ++i) {
    std::string L = s[i];
    if (L == "@") { break;  }
    Cuss::prtL(L.c_str());
  }

  // Movement keys:
  const int oy = 1;
  COLORREF moveHue = RGB(0, 185, 0);
  Cuss::setCellBg(CPoint(15, 3 + oy), moveHue); // H
  Cuss::setCellBg(CPoint(17, 3 + oy), moveHue); // J
  Cuss::setCellBg(CPoint(19, 3 + oy), moveHue); // K
  Cuss::setCellBg(CPoint(21, 3 + oy), moveHue); // L

  Cuss::setCellBg(CPoint(14, 4 + oy), moveHue); // BN
  Cuss::setCellBg(CPoint(16, 4 + oy), moveHue);

  Cuss::setCellBg(CPoint(14, 2 + oy), moveHue); // YU
  Cuss::setCellBg(CPoint(16, 2 + oy), moveHue);

  COLORREF oHue = RGB(185, 0, 185); // commands I could remove!
  COLORREF iHue = RGB(185, 0, 0);

  // O is Open:
  // (p is unequip - why/how?)
  Cuss::setCellBg(CPoint(20, 2 + oy), iHue); // O,open/close(!)
  Cuss::setCellBg(CPoint(18, 2 + oy), oHue); // I,inventory // COULD be dropped! (A instead)
  Cuss::setCellBg(CPoint(12, 2 + oy), iHue); // T is take off
  Cuss::setCellBg(CPoint(8, 2 + oy), iHue); // E examine

  Cuss::setCellBg(CPoint(6, 2 + oy), oHue); // W = wear // COULD be dropped!(A instead)
  Cuss::setCellBg(CPoint(4, 2 + oy), iHue); // Q = stats

  Cuss::setCellBg(CPoint(5, 3 + oy), iHue); // A = activate-use
  Cuss::setCellBg(CPoint(7, 3 + oy), iHue); // S see/look
  Cuss::setCellBg(CPoint(9, 3 + oy), iHue); // D drop
  Cuss::setCellBg(CPoint(11, 3 + oy), iHue); // F bash
  Cuss::setCellBg(CPoint(13, 3 + oy), iHue); // G get

  Cuss::setCellBg(CPoint(6, 4 + oy), iHue); // Z zap=cast spell
  Cuss::setCellBg(CPoint(8, 4 + oy), oHue); // X spell-book(not nec?) // COULD be dropped! (z instead.)
  Cuss::setCellBg(CPoint(10, 4 + oy), iHue); // C stairs, go up/down


	TheUI::promptForAnyKey(__FILE__, __LINE__, "help-pause");
	Cuss::clear(true);
	return true;
}
