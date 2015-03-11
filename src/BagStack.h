#pragma once

#include "Bag.h"
#include <map>

class Stacker {
public:
  int objIx;
  SpellEnum spell;

  Stacker(Obj* o) {
    spell = o->effect;
    objIx = Obj::def2ix(o->objdef);
  }
};

typedef std::vector< Obj* > Coll;

class BagStack
{
public:

  std::map< Stacker, Coll > stacks;

  BagStack(Bag& bag);

  BagStack();
  ~BagStack();
};

