#pragma once

#include "cellmap/cellmap.h"
#include "MobQueue.h"


class Dungeon
{
public:
  int level;
  Map map;
  MobQueue mobs;

  Dungeon(int level_);
  ~Dungeon();

  void initDungeon();
  void initPlayerForMap(); // JG, All this shouldn't really clutter Map/CellMap -'initWorld'  should go somewhere OUTSIDE basic structures' impls.
  void initMobs();

  bool persist(class Persist& p);
};

