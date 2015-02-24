#include "stdafx.h"
#include "Persist.h"


Persist::Persist(std::ostream& os_)
:bOut(true)
,os(os_)
,is("basin2.sav")
{
}


Persist::~Persist()
{
}
