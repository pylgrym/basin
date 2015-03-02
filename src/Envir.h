#pragma once

#include "numutil/myrnd.h"
#include <ostream>

#include "Persist.h"

enum EnvirEnum {
  EN_Floor='.',
  EN_Wall='#',
  EN_Border='&',
  EN_Vein='%',
  EN_StairUp = '<',
  EN_StairDown = '>',
  EN_Shop = 'S',

  EN_Tree = 'T',
  EN_Green = 'G',
  EN_Water = 'W',

  EN_MaxLimit // highest nr to size arrays.
};

class Envir {
public:
  EnvirEnum type;
  bool permLight;
  int envUnits; // E.g. strength of rock.

  bool persist(class Persist& p) {
    // JG: enums are problematic, require a template or something, or manual approach.
    // Do I want fixed-width BD AF hex numbers? (for grid appearance and easier parsing.)
    if (p.bOut) {
      p.os << (char) type << " "; // p.transfer(type, "envir"); // p.os << type;
    } else {
      // int intType = 0;
      char charType = 0;
      p.is >> charType;
      type = (EnvirEnum) charType;
    }
    return true;
  }

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

  bool interacts() const {  // FIXME, make a differentiate-mechanism
    return (type == EN_Shop);
  }

  void setType(EnvirEnum type_) { type = type_;  }
  const TCHAR* typeS() { return etypeAsStr(type);  }
  static const TCHAR* etypeAsStr(EnvirEnum type);
  static bool isBlockingEnv(EnvirEnum type); // May be put in different class.
};

