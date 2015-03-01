#include "stdafx.h"
#include "Bag.h"

#include "Cuss.h"

#include "theUI.h"

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
    totalWeight += o.weight;
  }
  return totalWeight;
}



void Bag::showInv() {
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

    /*
    CString desc = o.typeAsDesc(o.otype());
    desc.Replace(L".", L"a"); // FIXME: 'an' for vowels..
    CT2A descA(desc, CP_ACP);
    */
    std::string descA = o.an_item();

    // CT2A abbrA(o.typeAsStr(o.otype()), CP_ACP);  

    ss 
      // << " " << abbrA[0] // Icon-letter first.
      // << " " 
      << ix 
      << " " << o.weight/10.0  
      << " " << descA
    ;
    Cuss::prtL(ss.str().c_str());  
    totalWeight += o.weight;
  }

  std::stringstream ss;
  ss << "Total weight: " << totalWeight / 10.0 << " kg";
  ss << " (" << everything/10.0 << ")";
  Cuss::prtL(ss.str().c_str());  

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



Obj* Bag::pick(const char* prompt) {
  Cuss::clear(false);
  Cuss::prtL(prompt); 

  showInv(); // Bag::bag.
  const char firstKey = 'A';
  char lastKey = firstKey + objs.size()-1; // Bag::bag.
  char lower = lastKey - ('A' - 'a');
  CString s; 
  s.Format(L"(letter [a-%c] or ESC)", lower);
  CT2A keyPrompt(s, CP_ACP);  

  int key = 0;
  for (;;) {
    key = TheUI::promptForKey(keyPrompt); 
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
