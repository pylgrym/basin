#include "stdafx.h"
#include "Bag.h"
#include "Cuss.h"
#include "theUI.h"
#include <iomanip>
#include <assert.h>
#include "BagStack.h"

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
    Cuss::prtL("Zero items in bag."); // Nothing.Nada."); 
  }

  char ix = 'a';

  double everything = bagWeight() + Equ::worn.wornWeight();

  double totalWeight = 0;
  BagCont::iterator i;
  // FIXME - bag and iterators should be prominent, so you get 'Bag::begin'.
  for (i = objs.begin(); i != objs.end(); ++i, ++ix) {
    Obj& o = **i;
    std::stringstream ss;
    std::string descA = o.indef_item(); // an_item();
    ss << ix;
    if (bShowPrice) {
      ss << " " << o.price() << "g";
    } else {
      ss << " " << std::fixed << std::setprecision(1) << o.kweight();
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



void Bag::showBagInvStacked(bool bShowPrice) {
  BagStack stack(*this);
  showBagInvStackedImpl(stack, bShowPrice);
}


void Bag::showBagInvStackedImpl(BagStack& stack, bool bShowPrice) {
  if (objs.size() == 0) {
    Cuss::prtL("Zero items in bag (stacked)."); // Nothing.Nada."); 
  }

  bool evenRow = true;
  
  char ix = 'a';
  double everything = bagWeight() + Equ::worn.wornWeight();
  double totalWeight = 0;
  StackMap::iterator i; // BagCont::iterator i;
  for (i = stack.stacks.begin(); i != stack.stacks.end(); ++i, ++ix, evenRow=!evenRow) {

    Obj& o = *stack.rep(i); 
    int count = stack.count(i);
    std::stringstream ss;
    std::string descA = o.indef_item(); 
    ss << ix;
    ss << " " << count;

    double kilos = stack.weights(i);
    if (bShowPrice) {
      ss << " " << o.price() << "g";
    } else {
      ss << " " << std::fixed << std::setprecision(1) << kilos; // o.kweight();
    }
    ss << " " << descA;

    PushBkCol bk(evenRow ? RGB(64, 50, 50) : RGB(50, 64, 64));
    PushFgCol fg(evenRow ? RGB(255, 200, 200) : RGB(200, 255, 255));
    Cuss::prtL(ss.str().c_str());  

    totalWeight += kilos;
  }

  if (!bShowPrice) { // Then show weights instead:
    std::stringstream ss;
    ss << "Total weight: " << totalWeight << " kg";
    ss << " (" << everything << ")";
    Cuss::prtL(ss.str().c_str());  
  }

  Cuss::invalidate();
}



char Bag::letterIxStacked(Obj* item) { // follows stacker
  BagStack stack(*this); // hack/kludge.

  char ix = 'a';
  StackMap::iterator si;
  for (si = stack.stacks.begin(); si != stack.stacks.end(); ++si, ++ix) {
    Coll& coll = si->second;
    Coll::iterator j;
    for (j = coll.begin(); j != coll.end(); ++j) {
      Obj* o = *j;
      if (o == item) {
        return ix;
      }
    }
  }
  return '?';
}

Obj* Bag::pickActionStacked(class BagStack& stack) { 
  const char firstKey = 'A';
  char lastKey = firstKey + stack.stacks.size()-1; // Bag::bag.
  char lower = lastKey - ('A' - 'a');
  CString s; 
  s.Format(L"(letter [a-%c] or ESC)", lower);
  CT2A keyPrompt(s, CP_ACP);  

  int key = 0;
  for (;;) {
    key = TheUI::promptForKey(keyPrompt, __FILE__, __LINE__, "pick-itemb"); 
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

  StackMap::iterator si =stack.stacks.begin(); 
  // BagCont::iterator si = objs.begin(); 

  for (int i = 0; i < objIx && si != stack.stacks.end(); ++i, ++si) { }
  if (si == stack.stacks.end()) { return false;  } // Error condition.

  // Obj* obj = *si;
  Obj* obj = stack.rep(si); 

  debstr() << "You picked:" << obj << "\n";

  Cuss::clear(true);
  return obj;
}



Obj* Bag::findItem(ObjEnum otype) {
  BagCont::iterator si; 
  for (si = objs.begin(); si != objs.end(); ++si) {
    Obj* obj = *si;
    if (obj->otype() == otype) { return obj;  }
  }
  return NULL;
}


char Bag::letterIxOld(Obj* item) { // Must be fixed to follow stacker
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



Obj* Bag::pickBag(const char* prompt, bool bShowPrice) {
  Cuss::clear(false);
  Cuss::prtL(prompt);

  bool oldstyle = false;
  Obj* choice = NULL;
  if (oldstyle) {
    showBagInv(bShowPrice); // in pickbag.
    choice = pickAction();
  } else {
    BagStack stack(*this);
    showBagInvStackedImpl(stack, bShowPrice); // in pickbag.
    choice = pickActionStacked(stack);
  }

  return choice;
}



Obj* Bag::pickAction() { 
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
    Cuss::prtL("Zero items in shop."); // Nothing.Nada.");
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
