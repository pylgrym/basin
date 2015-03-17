#include "stdafx.h"
#include "LOS.h"

// #include "Bresenham.h"

LOS::LOS() {
  initDisted();
}


LOS::~LOS()
{
}

bool operator < (CPoint a, CPoint b) {
  if (a.x != b.x) { return a.x < b.x;  }
  return  a.y < b.y;  
}


// void LOS::test() {}
