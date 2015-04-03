#pragma once

#include "dungeon.h"

namespace Dung {

extern Dungeon* CL; // 'Current Level' // should at least be in a namespace with 'using'.

class Dungeons
{
public:
  std::vector<Dungeon*> dungeons;
  Dungeons();
  ~Dungeons();

  static Dungeons the_dungeons;
  static Dungeon* get(int level);
  static Dungeon* setCurLevel(int level);

  void initDungeons(bool loadGame);

  void initNewGame();
  bool initLoadGame();

  bool persist(class Persist& p);

  private:
  Dungeon* implGet(int level);

};


}; // namespace.

using namespace Dung;
