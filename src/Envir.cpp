#include "stdafx.h"
#include "Envir.h"


#include <vector>


bool Envir::isBlockingEnv(EnvirEnum type) { // May be put in different class.
  switch (type) {
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
  }
  if (type < 0 || type >= (int) envirKeys.size()) { return L"out of bounds, envir enum.";  }
  return envirKeys[type];
}
