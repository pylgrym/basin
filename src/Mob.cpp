#include "stdafx.h"
#include "Mob.h"
#include "cellmap/cellmap.h"
#include "numutil/myrnd.h"
#include <assert.h>
#include "LogEvents.h"
#include "Equ.h"
#include <iomanip>
#include "Levelize.h"

#include "./theUI.h"

void Mob::placeMobOnMap(Map* map) {
  pos.x = rnd::Rnd(1, map->Width2 - 1);
  pos.y = rnd::Rnd(2, map->Height2 - 1); // FIXME, this '2' seems like outdated offset from when map and top-info-row were interacting?
}

Mob::Mob(int mlevel, bool bIsPlayer_)
:stats(mlevel,bIsPlayer_)
{
  mobDummyWeapon = Levelize::randDiceForLevel(mlevel);

  m_mobType = (CreatureEnum) rnd::Rnd(CR_Kobold, CR_MaxLimit);

  pos = CPoint(1, 1); // no map dimensions, so put mob on (1,1)

  color = RGB(rand()%255,rand()%255,rand()%255);

  defSchool = (AttackSchool) rnd::Rnd(0, SC_MaxSchools);
  mobSpell = Spell::rndSpell_level(mlevel);

  // starting-Mood distribution..
  mood = M_Sleeping; // The default..

  bool isSleepy = rnd::XinY(2,3); // 2 out of 3 mobs start out sleeping.
  if (!isSleepy) {
    bool drivenMoodType = rnd::oneIn(2);
    if (drivenMoodType) {
      mood = M_Driven;
    } else {  // old approach..

      bool badMood = rnd::oneIn(12);
      if (badMood) { 
        mood = (MoodEnum) rnd::Rnd(0, M_MaxMoods);
      } else { // Most monsters start out sleeping, or wandering.
        bool awake = rnd::oneIn(6);
        mood = (awake ? M_Wandering : M_Sleeping);
      }

    }
  } // if not default-sleeping.
  
}

/* idea: +2,+3 str / stat mod food, with 200-300 time expire timers (counters part of map.)
*/

/* idea - consider having 'grey floor' made with fill-brush and cell-colours.
Hmm, I do that currently, don't I?
*/

Mob::~Mob() {}



void Mob::invalidateGfx(CPoint pos, CPoint oldpos, bool force) {
  // JG: I'm not so sure about this method design, looks ugly to me. pos/oldpos is OK, but 'force' actually means 'another method' :-(.
  if (pos != oldpos || force) {
    TheUI::invalidateCell(oldpos); 
    TheUI::invalidateCell(pos); 
  }
}

void Mob::invalidateGfx() { // Hmm, these methods should get A/B names..?
  TheUI::invalidateCell(pos); 
}

const MobDef&  Mob::mobDef() {
  const MobDef& def = Creature::mobDef(ctype());
  return def;
}


