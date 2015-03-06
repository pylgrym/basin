#include "stdafx.h"
#include "Mob.h"
#include "cellmap/cellmap.h"
#include "numutil/myrnd.h"
#include <assert.h>
// #include "Cmds.h"
#include "LogEvents.h"
#include "Equ.h"
#include <iomanip>
#include "Levelize.h"

#include "./theUI.h"



Mob::Mob(int mlevel, bool bIsPlayer_)
  :stats(mlevel,bIsPlayer_)
{

  // mobDummyWeapon = Dice(rnd(3), rnd(2,12)); // Wow that can hit hard..
  mobDummyWeapon = Levelize::randDiceForLevel(mlevel);

  m_mobType = (CreatureEnum) rnd(CR_Kobold, CR_MaxLimit);

  pos.x = rnd(1, Map::Width-1); 
  pos.y = rnd(2, Map::Height-1);
  color = RGB(rand()%255,rand()%255,rand()%255);
  speed = 1.0;

  defSchool = (AttackSchool) rnd(0, SC_MaxSchools);

  bool badMood = oneIn(12);
  if (badMood) { 
    mood = (MoodEnum) rnd(0, M_MaxMoods);
  } else { // Most monsters start out sleeping, or wandering.
    bool awake = oneIn(6);
    mood = (awake ? M_Wandering : M_Sleeping);
  }
  
}


Mob::~Mob() {}



void Mob::invalidateGfx(CPoint pos, CPoint oldpos, bool force) {
  // JG: I'm not so sure about this method design, looks ugly to me. pos/oldpos is OK, but 'force' actually means 'another method' :-(.
  if (pos != oldpos || force) {
    TheUI::invalidateCell(oldpos); 
    TheUI::invalidateCell(pos); 
  }
}

void Mob::invalidateGfx() {
  TheUI::invalidateCell(pos); 
}






