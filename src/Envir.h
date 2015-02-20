#pragma once

#include "numutil/myrnd.h"

enum EnvirEnum {
  EN_Floor=0,
  EN_Wall=1,
  EN_Border=2,

  EN_Unv,
  EN_Vis,
  //EN_Wall,
  EN_Wall1,
  EN_Wall2,
  EN_Open,
  EN_Open2,
  EN_Vein,
  EN_MaxLimit // highest nr to size arrays.
};

class Envir {
public:
  EnvirEnum type;
  bool permLight;
  int envUnits; // E.g. strength of rock.

  Envir():type(EN_Floor), permLight(false) {
    envUnits = rnd(50, 1500);
  }

  bool blocked() const { return isBlockingEnv(type);  }

  void markWalls() { 
    if (type == EN_Wall) { // What about EN_Border? I don't really want to mark it, because it's a technical thing, not a 'game thing'.
      permLight = true;
    }
  }

  void lightCells() { // Used by light-spells.
    permLight = true;
    //if (type == EN_Wall) { // What about EN_Border? I don't really want to mark it, because it's a technical thing, not a 'game thing'.
    //}
  }

  void setType(EnvirEnum type_) { type = type_;  }
  const TCHAR* typeS() { return typeAsStr(type);  }
  static const TCHAR* typeAsStr(EnvirEnum type);
  static bool isBlockingEnv(EnvirEnum type); // May be put in different class.
};

