#pragma once

#include "cellmap/cellmap.h"

class Obj;

#include <set>
#include <vector>

//typedef std::set<Obj*> BagCont;
typedef std::vector<Obj*> BagCont;

class Bag
{
public:
  enum BagEnum { MaxBagSize = 10 };

  BagCont objs; // FIXME, change to vector, change 'add' to append!

  bool canAdd() const { return objs.size() < MaxBagSize;  }
  bool add(Obj* o, std::ostream& err);
  bool remove(Obj* obj, std::ostream& err);
  char letterIx(Obj* obj);

  void showInv();
  Obj* pick(const char* prompt);

  Obj* findItem(ObjEnum otype);

  Bag();
  ~Bag();

  static Bag bag;
};

