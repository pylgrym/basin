#pragma once

#include "cellmap/cellmap.h"
#include "MobQueue.h"

#include "LOS.h"

class Dungeon
{
public:
  int level;
  Map map;
  MobQueue mobs;

  LightMap lightmap;

  Dungeon(int level_);
  ~Dungeon();

  void initDungeon();
  void initPlayerForMap(); // JG, All this shouldn't really clutter Map/CellMap -'initWorld'  should go somewhere OUTSIDE basic structures' impls.
  void initMobs();

  bool persist(class Persist& p);
};

