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
  enum BagEnum { MaxBagSize = 20 }; // 15 }; //  10}; - 10 is too few..

  BagCont objs; // is vector.
  int size() const { return objs.size();  }
  bool empty() const { return objs.empty();  }

  bool canAdd() const { return objs.size() < MaxBagSize;  }
  bool add(Obj* o, std::ostream& err);
  bool remove(Obj* obj, std::ostream& err);
  void destroy(int ix);
  char letterIx(Obj* obj);

  void showBagInv(bool bShowPrice);
  void showBagInvStacked(bool bShowPrice);
  void showBagInvStackedImpl(class BagStack& stack, bool bShowPrice);

  void showShopInv();
  Obj* pickBag(const char* prompt, bool bShowPrice);
  Obj* pickAction(); // helper for pick 
  Obj* pickActionStacked(class BagStack& stack); // helper for pick 

  Obj* findItem(ObjEnum otype);

  double bagWeight();

  Bag();
  ~Bag();

  bool persist(class Persist& p);

  static Bag bag;
};

