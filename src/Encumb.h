#pragma once

class Encumb
{
public:
  enum EncumbEnum { LightE = 0, MediumE = 1, HeavyE = 4, CantLiftE = 10 }; // these values also act as penalties.

  Encumb();
  ~Encumb();

  static double totalWeight();
  static EncumbEnum calcEnc(double kilos, class Stats& stats);
  static EncumbEnum enc();
  static const char* encTxt(EncumbEnum type);
};

