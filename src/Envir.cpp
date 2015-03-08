#include "stdafx.h"
#include "Envir.h"


#include <vector>


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


const TCHAR* Envir::etypeAsStr(EnvirEnum type) {
  static std::vector<CString> envirKeys;
  if (envirKeys.size() == 0) {
    envirKeys.resize(EN_MaxLimit);
    envirKeys[EN_Floor] = L"."; // floor";
    envirKeys[EN_Wall] = L"#"; // wall";
    envirKeys[EN_Border] = L"border"; // @"; // border";
    envirKeys[EN_Vein] = L"vein"; //
    envirKeys[EN_StairUp] = L"stairup";
    envirKeys[EN_StairDown] = L"stairdown";
    envirKeys[EN_Shop] = L"shop"; 

    envirKeys[EN_Tree] = L"tree"; 
    envirKeys[EN_Green] = L"green"; 
    envirKeys[EN_Water] = L"waterlight"; 

    envirKeys[EN_DoorOpen]   = L"door_open"; 
    envirKeys[EN_DoorClosed] = L"door_closed";
    envirKeys[EN_DoorLocked] = L"door_locked";
    envirKeys[EN_DoorStuck]  = L"door_stuck";
    envirKeys[EN_DoorBroken] = L"door_broken";

    // envirKeys[EN_] = L""; 
    // envirKeys[EN_] = L""; 

    /// {OB_StairUp, EQ_None, ". stair up"},
    /// { OB_StairDown, EQ_None, ". stair down" },

    //envirKeys[EN_Unv]   = L"unv"; //
    //envirKeys[EN_Vis]   = L"vis"; //
    //envirKeys[EN_Wall1] = L"#"; //
    //envirKeys[EN_Wall2] = L"wall2"; //
    //envirKeys[EN_Open]  = L"open"; //
    //envirKeys[EN_Open2] = L"open2"; //


  }
  if (type < 0 || type >= (int) envirKeys.size()) { return L"out of bounds, envir enum.";  }
  return envirKeys[type];
}
