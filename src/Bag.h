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
  enum BagEnum { MaxBagSize = 15 }; //  10}; - 10 is too few..

  BagCont objs; // is vector.

  bool canAdd() const { return objs.size() < MaxBagSize;  }
  bool add(Obj* o, std::ostream& err);
  bool remove(Obj* obj, std::ostream& err);
  char letterIx(Obj* obj);

  void showBagInv();
  void showShopInv();
  Obj* pickBag(const char* prompt);
  Obj* pickAction(); // helper for pick 

  Obj* findItem(ObjEnum otype);

  double bagWeight();

  Bag();
  ~Bag();

  bool persist(class Persist& p);

  static Bag bag;
  static Bag shop; // hack
};

