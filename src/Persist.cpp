#include "stdafx.h"
#include "Persist.h"


Persist::Persist(std::ostream& os_)
:bOut(true)
//,dummy2("dummy")
,os(os_)
,is(dummy2) //"basin2.sav")
{
}

Persist::Persist(std::istream& is_)
  :bOut(false)
  , os(dummy_)
  , is(is_)
{
}


Persist::~Persist()
{
}
