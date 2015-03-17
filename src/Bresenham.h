#pragma once

#include <vector>

class Bresenham
{
public:
  Bresenham();
  ~Bresenham();
};


class BresIter {
public:
  std::vector<CPoint> pixels; // Output.
  int i; // loop var.

  int longest, shortest;
  int dx1, dy1, dx2, dy2;
  int numerator;
  int x, y;

  // The iterator interface - see iterAll2 below, for example.
  BresIter(int x1, int y1, int x2, int y2);
  bool done() const { return !(i <= longest);  }
  CPoint next();

  void iterAll1() { // Use this, to fill pixels-vector.
    for (int i=0; i<=longest; i++) {
      next();
    }
  }

  void iterAll2() {
    for ( ; !done(); ) {
      CPoint p = next();
      // Use p
    } 

    do { CPoint p = next(); } while (!done());
  }


}; // end class BresIter 

