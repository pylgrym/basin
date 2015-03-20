#pragma once
class FightDashboard
{
public:
  FightDashboard() { maxHP = 0; hp = 0; lastHit = 0; }
  ~FightDashboard();

  int maxHP;
  int hp;
  int lastHit;

  static FightDashboard dashboard;
};

