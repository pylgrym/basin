#include "stdafx.h"
#include "ShopInv.h"

#include "util/debstr.h"

Bag ShopInv::shop;

ShopInv::ShopInv()
{
}


ShopInv::~ShopInv()
{
}



void ShopInv::initShop() {
  // Create shop 
  std::stringstream ignore;
  ShopInv::shop.add(new Obj(Obj::objDesc(OB_Food), 1), ignore);
  ShopInv::shop.add(new Obj(Obj::objDesc(OB_Pickaxe), 1), ignore);

  Obj* firstLamp2 = new Obj(Obj::objDesc(OB_Lamp), 1);
  firstLamp2->itemUnits = 3700;

  ShopInv::shop.add(firstLamp2, ignore);
  ShopInv::shop.add(new Obj(Obj::objDesc(OB_LampOil), 1), ignore);
}


int ShopInv::shopTimer = 0;


void ShopInv::updateShop() { // restockshop
  const int interval = 50;
  if ((++shopTimer % interval) != 0) {
    return;
  }
  debstr() << "shop might update.\n";
  // mindstemanden
  const int maxShopItems = 20;
  if (!Rnd::oneIn(3)) {
    return;
  }
  debstr() << "shop will update.\n";

  int buf = (maxShopItems - shop.size());
  int bufmax = 6;
  if (buf < bufmax) { // If we have plenty, shuffle some.
    debstr() << "shop will shuffle.\n";
    if (Rnd::XinY(buf, bufmax)) {
      int n = Rnd::rndC(1, 3);
      removeN(n);
      int n2 = Rnd::rndC(1, 3);
      restockN(n2);
      return;
    }
  }

  // If we don't have plenty, just add some more.
  debstr() << "shop will add inv.\n";
  int n3 = Rnd::rndC(1, 5); // 3);
  restockN(n3);
}


void ShopInv::removeN(int n) { // Throw away n random shop items.
  for (int i = 0; i < n; ++i) {
    if (shop.empty()) { return;  }
    int ix = Rnd::rnd(shop.size());
    shop.destroy(ix);
  }
}

void ShopInv::restockN(int n) { // Add n random items to shop.
  for (int i = 0; i < n; ++i) {
    restock();
  }
}

void ShopInv::restock() {
  // Add a single "random" item: (but useful/necessary/in theme with shop)
  ObjEnum otype = Rnd::rnd(2) ? OB_Food : OB_LampOil;
  std::stringstream ignore;
  ShopInv::shop.add(new Obj(Obj::objDesc(otype), 1), ignore);

  // ShopInv::shop.add(new Obj(Obj::objDesc(OB_Food), 1), ignore);
  // ShopInv::shop.add(new Obj(Obj::objDesc(OB_LampOil), 1), ignore);
}
