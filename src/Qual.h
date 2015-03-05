#pragma once

#include "parser/ParseCSV.h"

struct QualItem {
  std::string name;
  COLORREF color;
};

class Qual
{
public:
  Qual();
  ~Qual();

  void move();

  std::vector< QualItem > quals;
  std::map< std::string, QualItem > qualmap;

  ParseObj data;

  static Qual qual;
};

