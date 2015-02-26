#include "stdafx.h"
#include "Dungeons.h"


Dungeons Dungeons::the_dungeons;

Dungeon* Dungeons::get(int level) {
  return the_dungeons.implGet(level);
}

Dungeon* Dungeons::implGet(int level) {
    // we start at level 1, not 0! (maybe there'll be a town level at 0.)
  if ((int)dungeons.size() <= level) {
    dungeons.resize(level+1);
  }
  if (dungeons[level] == NULL) {
    Dungeon* newDungeon = new Dungeon(level);
    newDungeon->initDungeon();
    dungeons[level] = newDungeon;
  }

  return dungeons[level];
}

bool Dungeons::persist(class Persist& p) {
  int dungeonDepth = dungeons.size();
  p.transfer(dungeonDepth, "dungeonDepth");
  if (!p.bOut) { dungeons.resize(dungeonDepth);  }

  int dungeonCount = 0;
  for (int i = 0; i < (int) dungeons.size(); ++i) {
    if (dungeons[i] != NULL) {
      ++dungeonCount;
    }
  }
  p.transfer(dungeonCount, "dungeonCount");
  if (p.bOut) { // Output
    for (int i = 0; i < dungeonDepth; ++i) {
      if (dungeons[i] != NULL) {
        p.transfer(i, "dungeonLevel");
        Dungeon& dungeon = *dungeons[i];
        dungeon.persist(p);
      }
    }
  } else { // Input.
    dungeons.resize(dungeonDepth);
    for (int i = 0; i < dungeonCount; ++i) {
      int dungeonLevel = 0;
      p.transfer(dungeonLevel, "dungeonLevel");
      Dungeon* dungeon = new Dungeon(dungeonLevel);
      dungeons[dungeonLevel] = dungeon;
      dungeon->persist(p);
    }
  }
  return true;
}


Dungeon* Dung::CL = NULL; // should at least be in a namespace with 'using'.


void Dungeons::setCurLevel(int level) {
  Dungeon* dung = get(level);
  Dung::CL = dung;
}


Dungeons::Dungeons()
{
}


Dungeons::~Dungeons()
{
}
