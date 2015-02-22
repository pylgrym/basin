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

  // everything else is not blocking movement..
  // case:
  // EN_Floor:  
  }
  return false;
}


const TCHAR* Envir::typeAsStr(EnvirEnum type) {
  static std::vector<CString> envirKeys;
  if (envirKeys.size() == 0) {
    envirKeys.resize(EN_MaxLimit);
    envirKeys[EN_Floor] = L"."; // floor";
    envirKeys[EN_Wall] = L"#"; // wall";
    envirKeys[EN_Border] = L"border"; // @"; // border";
    envirKeys[EN_Vein] = L"vein"; //
    envirKeys[EN_StairUp] = L"stairup";
    envirKeys[EN_StairDown] = L"stairdown";

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
