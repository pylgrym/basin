#include "stdafx.h"
#include "Qual.h"

#include <fstream>

#include "util/debstr.h"

Qual::Qual()
{
  std::ifstream is("color.csv");
  ParseCSV parser(is, data);
  parser.parse();

  move();
  debstr() << "count:" << quals.size() << std::endl;
}



bool hasDigit(const std::string& s) {
  std::string::const_iterator i;
  for (i = s.begin(); i != s.end(); ++i) {
    char c = *i;
    if (c >= '0' && c <= '9') { return true;  }
  }
  return false;
}


void Qual::move() {

  for (int i = 0; i < data.size(); ++i) {
    // PRow& row = data[i];
    // Color;Dummy;R;G;B
    QualItem item;
    item.name = data.field("Color", i);
    if (hasDigit(item.name)) { continue; } // skip those with numbers.
    item.color = RGB( data.Int("R", i), data.Int("G", i), data.Int("B", i) );
    quals.push_back(item);
    qualmap[item.name] = item;
  }
}


Qual::~Qual()
{
}

Qual Qual::qual;
