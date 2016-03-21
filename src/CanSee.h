#pragma once
class CanSee {
  // Utility class that uses Bresenham, to
  // check if there is LOS between two points.

public:
  CanSee();
  ~CanSee();

  static bool canSee(CPoint a, CPoint b, class Map& m, bool onlyEnvir);
};

