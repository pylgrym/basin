#pragma once
class CanSee
{
public:
  CanSee();
  ~CanSee();

  static bool canSee(CPoint a, CPoint b, class Map& m, bool onlyEnvir);
};

