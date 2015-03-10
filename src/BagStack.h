#pragma once

#include "Bag.h"
#include <map>

class Stacker {
public:
  int objIx;
  SpellEnum spell;

  bool operator < (const Stacker& rhs) const {
    if (objIx != rhs.objIx) { return objIx < rhs.objIx;  }
    return spell < rhs.spell;
  }

  Stacker(Obj* o) {
    spell = o->effect;
    objIx = Obj::def2ix(o->objdef);
  }
};

typedef std::vector< Obj* > Coll;
typedef std::map< Stacker, Coll > StackMap; 

class BagStack
{
public:

  StackMap stacks;

  BagStack(Bag& bag);
  BagStack();
  ~BagStack();

  void add(Obj* o) {
    Stacker stacker(o);
    stacks[stacker].push_back(o);
  }

  Obj* rep(StackMap::iterator i) {
    if (i != stacks.end()) {
      return i->second.front();
    }
    return NULL;
  }

  int count(StackMap::iterator i) {
    if (i != stacks.end()) {
      return i->second.size();
    }
    return 0;
  }

  double weights(StackMap::iterator i) {
    double kilos = 0.0;
    if (i != stacks.end()) {
      Coll::iterator j;
      for (j = i->second.begin(); j != i->second.end(); ++j) {
        kilos += (*j)->kweight();
      }
    }
    return kilos;
  }
};

