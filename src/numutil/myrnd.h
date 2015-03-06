#ifndef MY_RND_H
#define MY_RND_H 

#include <vector>
// I eventually should put them in a namespace and combine with 'using';
// I DON'T want to write longPrefixCrap::rnd..

struct Dice {
  Dice() { n = 1; x = 2;  }
  Dice(int n_, int x_) :n(n_), x(x_) {}
  int n; // number of dice.
  int x; // dice size.
  int roll(std::ostream& os) const;
};

extern int rnd(int range); // from 0 to (range-1), evenly distributed.
extern int rnd(int from, int to); // from, NOT including 'to'. 
extern int rndC(int from, int to); // from, AND including 'to'.  ("Closed")

extern bool oneIn(int Nth); // "one out of N", e.g. 1/6 chance.
extern bool XinY(int x, int y); // X out of Y

extern int Dx(int x); // 1 x-die.
extern int nDx(int n, int x); // roll n Dice-x and add them. 
extern int nDx_ss(int n, int x, std::ostream& os); // roll n Dice-x and add them. 
extern int nDxDetail(int n, int x, std::vector<int>& details); // roll n Dice-x and add them. 

#endif // MY_RND_H.