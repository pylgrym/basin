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

  EN_DoorOpen   = '1',
  EN_DoorClosed = '2',
  EN_DoorLocked = '3',
  EN_DoorStuck  = '4',
  EN_DoorBroken = '5',
  EN_DoorOneWay = '6', // (IDEA) Only allows passage in one direction (how to control?)
  EN_DoorCullis = '7', // (IDEA) Shuts after first passage through. // It's 'portcullis', but I hate long words..

  EN_MaxLimit = 127 // highest nr to size arrays.
};

class Envir {
public:
  EnvirEnum type;
  bool permLight;
  int envUnits; // E.g. strength of rock.

  int tmpLightStr; // embellishes permLight, but is not currently saved.

  bool persist(class Persist& p) {
    // JG: enums are problematic, require a template or something, or manual approach.
    // Do I want fixed-width BD AF hex numbers? (for grid appearance and easier parsing.)
    if (p.bOut) {
      p.os << (char)type;
      p.os << " "; 
      p.os << (permLight ? ',' : '.');
      p.os << " "; 

    } else { // input.
      char charType = 0;
      p.is >> charType;
      type = (EnvirEnum) charType;

      
      p.is >> charType;
      permLight = (charType == ',');
      
    }
    return true;
  }

  Envir():type(EN_Floor), permLight(false) {
    envUnits = Rnd::rnd(50, 1500);
    tmpLightStr = 0; // 14;
  }

  bool blocked() const { return isBlockingEnv(type);  }

  void markWalls(CPoint pos);

  void lightCells(CPoint pos); // Used by light-spells.

  bool interacts() const {  // FIXME, make a differentiate-mechanism
    return (type == EN_Shop);
  }

  static EnvirEnum ranDoor();

  bool isDoor() const {
    switch (type) {
    case EN_DoorOpen:
    case EN_DoorClosed:
    case EN_DoorLocked:
    case EN_DoorStuck:
    case EN_DoorBroken:
    case EN_DoorOneWay: case EN_DoorCullis: // these two are experimental.
      return true;
    }
    return false; // Everything else is not a door..
  }

  bool isStair() const {
    switch (type) {
    case EN_StairDown:
    case EN_StairUp:
      return true;
    }
    return false; // Everything else is not a door..
  }

  void setType(EnvirEnum type_) { type = type_;  }
  const TCHAR* typeS() { return etypeAsStr(type);  }
  const std::string typestr();
  static const TCHAR* etypeAsStr(EnvirEnum type);
  static bool isBlockingEnv(EnvirEnum type); // May be put in different class.
};

