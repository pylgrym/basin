#include "stdafx.h"
#include "myrnd.h"

#include <ostream>

int rnd(int range) { return rand() % range;  }

int rnd(int from,int to) { 
  int delta = to - from;
  return from + rand() % delta; // (to - from);
} // from, NOT including 'to'. 

int rndC(int from,int toC) { return from + rand() % (1+toC-from);  } // from, AND including 'to'. 
bool oneIn(int Nth) { return (rand() % Nth) == 0;  } // "one out of N", e.g. 1/6 chance.

int Dx(int x) { // 1 x-die.
  return rnd(1, x);
}

int nDx(int n, int x) { // roll n Dice-x and add them. 
  int sum = 0;
  for (int d = 1; d <= n; ++d) {
    int roll = Dx(x);
    sum += roll;
  }
  return sum;
}

extern int nDx_ss(int n, int x, std::ostream& os) { // roll n Dice-x and add them. 
  int sum = 0;
  for (int d = 1; d <= n; ++d) {
    int roll = Dx(x);
    if (d > 1) { os << "+";  }
    os << roll;
    sum += roll;
  }
  os << "=" << sum;
  return sum;
}

int nDxDetail(int n, int x, std::vector<int>& details) { // roll n Dice-x and add them. 
  /* Does the same as nDx, but let's you see the details (e.g. to print 2+4+1, so you can see why you are getting those ugly numbers :-).
  NB, I might want to gather-show the intermediate results.
  */
  int sum = 0;
  for (int d = 1; d <= n; ++d) {
    int roll = Dx(x);
    details.push_back(roll);
    sum += roll;
  }
  return sum;
}



int Dice::roll(std::ostream& os) const {
  int result = nDx_ss(n, x, os); //nDx
  return result;
}

