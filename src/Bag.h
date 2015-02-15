#pragma once

#include "cellmap/cellmap.h"

#include <set>

class Bag
{
public:
  enum BagEnum { MaxBagSize = 10 };

  std::set<Obj*> objs;

  bool canAdd() const { return objs.size() < MaxBagSize;  }
  bool add(Obj* o, std::ostream& err);
  bool remove(Obj* obj, std::ostream& err);

  void showInv();
  Obj* pick(const char* prompt);

  Bag();
  ~Bag();

  static Bag bag;
};

