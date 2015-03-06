#include "stdafx.h"
#include "Bag.h"
#include "Cuss.h"
#include "theUI.h"
#include <iomanip>
#include <assert.h>

Bag::Bag()
{
}


Bag::~Bag()
{
}

Bag Bag::bag;


bool Bag::add(Obj* o, std::ostream& err) {
  if (objs.size() >= MaxBagSize) {
    err << "You have too many items in your bag.";
    return false;
  }
  // FIXME - consider a fixed order of items/slots, so you always fill empty spaces.
  objs.push_back(o); //  append(o); // insert(o);
  return true; 
}



bool Bag::remove(Obj* obj, std::ostream& err) {
  BagCont::iterator i;

  //i = objs.find(obj);
  i = std::find(objs.begin(), objs.end(), obj); // objs.find(obj);

  if (i == objs.end()) { err << "bag: Err, obj not found?"; return false; }
  objs.erase(i);
  return true;
}

void Bag::destroy(int ix) {
  assert(ix >= 0 && ix < (int) objs.size());
  Obj* o = objs[ix];
  BagCont::iterator i = objs.begin() + ix;
  objs.erase(i);
  delete o;
}



char Bag::letterIx(Obj* item) {
  char ix = 'a';
  BagCont::iterator i;
  for (i = objs.begin(); i != objs.end(); ++i, ++ix) {
    Obj* o = *i;
    if (o == item) {
      return ix;
    }
  }
  return '?';
}

double Bag::bagWeight() {
  double totalWeight = 0;
  BagCont::iterator i;
  for (i = objs.begin(); i != objs.end(); ++i) {
    Obj& o = **i;
    totalWeight += o.kweight();
  }
  return totalWeight;
}



void Bag::showBagInv(bool bShowPrice) {
  if (objs.size() == 0) {
    Cuss::prtL("Zero items. Nothing. Nada."); 
  }

  char ix = 'a';

  double everything = bagWeight() + Equ::worn.wornWeight();

  double totalWeight = 0;
  BagCont::iterator i;
  // FIXME - bag and iterators should be prominent, so you get 'Bag::begin'.
  for (i = objs.begin(); i != objs.end(); ++i, ++ix) {
    Obj& o = **i;
    std::stringstream ss;
    std::string descA = o.an_item();
    ss << ix;
    if (bShowPrice) {
      ss << " " << o.price() << "g";
    } else {
      ss << " " << o.kweight();
    }
    ss << " " << descA;
    
    Cuss::prtL(ss.str().c_str());  
    totalWeight += o.kweight();
  }

  if (!bShowPrice) { // Then show weights instead:
    std::stringstream ss;
    ss << "Total weight: " << totalWeight << " kg";
    ss << " (" << everything << ")";
    Cuss::prtL(ss.str().c_str());  
  }

  Cuss::invalidate();
}



Obj* Bag::findItem(ObjEnum otype) {
  BagCont::iterator si; 
  for (si = objs.begin(); si != objs.end(); ++si) {
    Obj* obj = *si;
    if (obj->otype() == otype) { return obj;  }
  }
  return NULL;
}



Obj* Bag::pickBag(const char* prompt, bool bShowPrice) {
  Cuss::clear(false);
  Cuss::prtL(prompt);

  showBagInv(bShowPrice); // Bag::bag.
  return pickAction();
}



Obj* Bag::pickAction() { // const char* prompt) {
    const char firstKey = 'A';
  char lastKey = firstKey + objs.size()-1; // Bag::bag.
  char lower = lastKey - ('A' - 'a');
  CString s; 
  s.Format(L"(letter [a-%c] or ESC)", lower);
  CT2A keyPrompt(s, CP_ACP);  

  int key = 0;
  for (;;) {
    key = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-item"); 
    if (key == VK_ESCAPE) {
      Cuss::clear(true);
      return NULL; // Cancelled pick operation.
    }
    if (key >= firstKey && key <= lastKey) {
      break;
    }
    TheUI::BeepWarn();
  }

  int objIx = key - firstKey;
  // Approach necessary because sets are not indexable:
  BagCont::iterator si = objs.begin(); 
  for (int i = 0; i < objIx && si != objs.end(); ++i, ++si) { }
  if (si == objs.end()) { return false;  } // Error condition.
  Obj* obj = *si;

  debstr() << "You picked:" << obj << "\n";

  Cuss::clear(true);
  return obj;
}



bool Bag::persist(class Persist& p) {
  int bagCount = objs.size();
  p.transfer(bagCount, "bagCount");

  if (!p.bOut) { objs.resize(bagCount);  }

  const ObjDef& dummy = Obj::objDesc(OB_None);
  for (int i = 0; i < bagCount; ++i) {
    if (!p.bOut) { objs[i] = new Obj(dummy, 1); }
    Obj* o = objs[i];
    CPoint unused;
    o->persist(p, unused);
  }
  return true;
}




void Bag::showShopInv() {
  if (objs.size() == 0) {
    Cuss::prtL("Zero items. Nothing. Nada.");
  }

  char ix = 'a';
  BagCont::iterator i;
  for (i = objs.begin(); i != objs.end(); ++i, ++ix) {
    Obj& o = **i;
    std::stringstream ss;
    std::string descA = o.an_item();

    ss << ix;
    ss // << std::fixed << std::setw(4) 
       << " " << 
       o.price() << "g";

    ss << " " << descA;

    Cuss::prtL(ss.str().c_str());
  }

  // Cuss::prtL("B to buy, S to sell..");
  Cuss::invalidate();
}
