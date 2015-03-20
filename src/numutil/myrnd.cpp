#include "stdafx.h"
#include "myrnd.h"

#include <ostream>
#include <assert.h>

int rnd::Rnd(int range) { 
  if (range < 1) {
    DebugBreak();
  }
  assert(range != 0);
  // if (range == 0) { return 0; }
  return rand() % range;  
}

int rnd::Rnd(int from,int to) { 
  int delta = to - from;
  return from + rand() % delta; // (to - from);
} // from, NOT including 'to'. 

int rnd::rndC(int from,int toC) { return from + rand() % (1+toC-from);  } // from, AND including 'to'. 
bool rnd::oneIn(int Nth) { return (rand() % Nth) == 0;  } // "one out of N", e.g. 1/6 chance.

bool rnd::XinY(int x, int y) { return (rnd::Rnd(y) < x); }

bool rnd::pctChance(int pct) { return (rnd::Rnd(100) < pct); }

int rnd::Dx(int x) { // 1 x-die.
  return rnd::rndC(1, x);
}

int rnd::nDx(int n, int x) { // roll n Dice-x and add them. 
  int sum = 0;
  for (int d = 1; d <= n; ++d) {
    int roll = rnd::Dx(x);
    sum += roll;
  }
  return sum;
}

int rnd::nDx_ss(int n, int x, std::ostream& os) { // roll n Dice-x and add them. 
  int sum = 0;
  for (int d = 1; d <= n; ++d) {
    int roll = rnd::Dx(x);
    if (d > 1) { os << "+";  }
    os << roll;
    sum += roll;
  }
  os << "=" << sum;
  return sum;
}

int rnd::nDxDetail(int n, int x, std::vector<int>& details) { // roll n Dice-x and add them. 
  /* Does the same as nDx, but let's you see the details (e.g. to print 2+4+1, so you can see why you are getting those ugly numbers :-).
  NB, I might want to gather-show the intermediate results.
  */
  int sum = 0;
  for (int d = 1; d <= n; ++d) {
    int roll = rnd::Dx(x);
    details.push_back(roll);
    sum += roll;
  }
  return sum;
}



int Dice::roll(std::ostream& os) const {
  int result = rnd::nDx_ss(n, x, os); //nDx
  return result;
}

