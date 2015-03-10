#include "stdafx.h"
#include "BagStack.h"


BagStack::BagStack()
{
}


BagStack::~BagStack()
{
}

BagStack::BagStack(Bag& bag)
{
  BagCont::iterator i;
  for (i = bag.objs.begin(); i != bag.objs.end(); ++i) {
    Obj& o = **i;
    add(&o);
  }
}

