#pragma once

#include "Bag.h"

class ShopInv
{
public:
  ShopInv();
  ~ShopInv();

  static Bag shop; // hack
  static int shopTimer;

  static void initShop();
  static void updateShop(); // restockShop

  static void removeN(int n); // Throw away n random shop items.
  static void restockN(int n); // Add n random items to shop.
  static void restock();

};

