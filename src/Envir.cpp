#include "stdafx.h"
#include "Envir.h"
#include "PlayerMob.h"
#include <vector>

#include "theUI.h"
#include "sprites/Tilemap.h"

bool Envir::isBlockingEnv(EnvirEnum type) { // May be put in different class.
  switch (type) {

    // Begin maze-stuff:
  case EN_Vein: 
    // This is 'original wall': (of labyrinth - before we start filling tunnels.)
    // case EN_Unv: //M_Unvisited:
    // These two are used to fill tunnels:
  //case EN_Wall: //M_Wall:
  //case EN_Wall2: //M_Wall_H:
    return true;

    // These two are open space:
  //case EN_Vis: // M_Visited:
  case EN_Floor: //Open: //M_Open:

  //case EN_Open2: //M_OpenB:   // JG: confused if ever used? will look like cobweb.
    return false;
    // end maze-stuff:

  case EN_Wall:
  case EN_Border:
    return true; 

  case EN_DoorOpen:   case EN_DoorBroken: return false;  // they don't block..
  case EN_DoorClosed: case EN_DoorLocked: case EN_DoorStuck: return true; // they block..


  // everything else is not blocking movement..
  // case:
  // EN_Floor:  
  }
  return false;
}


const std::string Envir::typestr() {
  return etypeAsStr(type); // typeS();
}

void Envir::initEnvirs(Tiles& tiles) {
  if (priv_envirKeys.size() == 0) { initEnvirKeys();  }

  for (int i = 0; i < EN_MaxLimit; ++i) {
    std::string key = priv_envirKeys[i];
    if (key.empty()) { continue; }

    EnvirDef& def = envirs[i];
    def.desc = key;

    // init cpoint tilekey:
    CA2T ukey(key.c_str(), CP_ACP);
    CPoint tilePos = tiles.tile(CString(ukey)); 
    def.tilekey = tilePos;
  }
}


void Envir::initEnvirKeys() {
  if (priv_envirKeys.size() == 0) {
    priv_envirKeys.resize(EN_MaxLimit);
    priv_envirKeys[EN_Floor]  = "."; // floor";
    priv_envirKeys[EN_Wall]  = "#"; // wall";
    priv_envirKeys[EN_Border]  = "border"; // @"; // border";
    priv_envirKeys[EN_Vein]  = "vein"; //
    priv_envirKeys[EN_StairUp]  = "stairup";
    priv_envirKeys[EN_StairDown]  = "stairdown";
    priv_envirKeys[EN_Shop]  = "shop"; 

    priv_envirKeys[EN_Tree]  = "tree"; 
    priv_envirKeys[EN_Green]  = "green"; 
    priv_envirKeys[EN_Water]  = "waterlight"; 

    priv_envirKeys[EN_DoorOpen]    = "door_open"; 
    priv_envirKeys[EN_DoorClosed]  = "door_closed";
    priv_envirKeys[EN_DoorLocked]  = "door_locked";
    priv_envirKeys[EN_DoorStuck]   = "door_stuck";
    priv_envirKeys[EN_DoorBroken]  = "door_broken";

    priv_envirKeys[EN_DoorOneWay]  = "door_oneway";
    priv_envirKeys[EN_DoorCullis]  = "door_cullis";

    // priv_envirKeys[EN_]  = ""; 
    // priv_envirKeys[EN_]  = ""; 

    /// {OB_StairUp, EQ_None, ". stair up"},
    /// { OB_StairDown, EQ_None, ". stair down" },

    //priv_envirKeys[EN_Unv]    = "unv"; //
    //priv_envirKeys[EN_Vis]    = "vis"; //
    //priv_envirKeys[EN_Wall1]  = "#"; //
    //priv_envirKeys[EN_Wall2]  = "wall2"; //
    //priv_envirKeys[EN_Open]   = "open"; //
    //priv_envirKeys[EN_Open2]  = "open2"; //
  }
}



std::vector<std::string> Envir::priv_envirKeys;

std::string Envir::etypeAsStr(EnvirEnum type) {
  if (priv_envirKeys.size() == 0) { initEnvirKeys(); }

  if (type < 0 || type >= (int) priv_envirKeys.size()) { return "out of bounds, envir enum.";  }
  return priv_envirKeys[type];
}


EnvirEnum Envir::ranDoor() {
  const int DoorTypeCount = 7;
  int choice = rnd::Rnd(DoorTypeCount);
  switch (choice) { // FIXME: locked/stuck must become more rare! (i'm tired of having to kick doors..)
  case 0: return EN_DoorOpen;
  case 1: return EN_DoorClosed;
  case 2: return EN_DoorLocked;
  case 3: return EN_DoorStuck;
  case 4: return EN_DoorBroken;
  case 5: return EN_DoorOneWay;
  case 6: return EN_DoorCullis; // these two are experimental.
  }
  return EN_DoorOpen;
}



/* idea, LOS check! canSee.
if map.canSee(pos,pos)
*/

void Envir::markWalls(CPoint pos) { 
  if (type == EN_Wall || type == EN_Border) { // What about EN_Border? I don't really want to mark it, because it's a technical thing, not a 'game thing'.      
    permLight = true;
  }
  TheUI::invalidateCell(pos);
}

void Envir::lightCells(CPoint pos) { // Used by light-spells.
  permLight = true;
  TheUI::invalidateCell(pos);

  //if (type == EN_Wall) { // What about EN_Border? I don't really want to mark it, because it's a technical thing, not a 'game thing'.
  //}

  int newStr = PlayerMob::distPly(pos); // consider  distPlyLight
  newStr = newStr/10; // / 10;
  if (tmpLightStr == 0 || newStr < tmpLightStr) { // Don't dampen existing/earlier light..
    tmpLightStr = newStr;
  }

}



EnvirDef Envir::envirs[EN_MaxLimit];

const EnvirDef& Envir::envDef(EnvirEnum etype) {
  assert(etype >= 0);
  assert(etype < EN_MaxLimit);
  return envirs[etype];
}

EnvirDef& Envir::envDefNC(EnvirEnum etype) {
  assert(etype >= 0);
  assert(etype < EN_MaxLimit);
  return envirs[etype];
}
