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


void Persist::eatLabel(std::istream& is, const std::string& expected) {
  std::string dummy; is >> dummy;
  if (dummy != expected) {
    debstr() << "Error: expected [" << expected << "], but got [" << dummy << "] instead.\n";
  }
}
