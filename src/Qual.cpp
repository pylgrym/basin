#include "stdafx.h"
#include "Qual.h"

#include <fstream>

#include "util/debstr.h"
#include <assert.h>

/* Good colours for flavor:
http://www.rapidtables.com/web/color/RGB_Color.htm
maroon
brown
crimson
red
coral
orange red
dark orange
orange
gold
khaki
olive
yellow
yellow green
olive drab
chartreuse
lime
light green
pale green
spring green
sea green
aqua marine
teal
cyan
turquoise
pale turquoise
powder blue
cadet blue
steel blue
corn flower blue
dodger blue
sky blue
midnight blue
navy
blue
royal blue
blue violet
indigo
slate blue
purple
thistle
plum
violet
magenta
orchid
deep pink
hot pink
pink
beige
bisque
wheat
corn silk
lemon chiffon
sienna
chocolate
peru
sandy brown
burly wood
tan
moccasin
misty rose
lavender
ivory
azure
black
grey
silver
white
*/

Qual::Qual()
{
  std::ifstream is("parser\\color.csv");
  ParseCSV parser(is, data);
  parser.parse();

  move(parser);
  debstr() << "qual count:" << quals.size() << std::endl;


}



bool hasDigit(const std::string& s) {
  std::string::const_iterator i;
  for (i = s.begin(); i != s.end(); ++i) {
    char c = *i;
    if (c >= '0' && c <= '9') { return true;  }
  }
  return false;
}

bool hasModTerm(const std::string& s) {
  if (s.find("edium") != s.npos) { return true;  }
  return false;
}

void Qual::move(ParseCSV& parser) {


  for (int i = 0; i < data.size(); ++i) {
    PRow& row = data[i];
    // Color;Dummy;R;G;B
    QualItem item;
    item.name = data.field("Color", i);
    if (hasDigit(item.name)) { continue; } // skip those with numbers.
    if (hasModTerm(item.name)) { continue; } // skip those with numbers.
    item.color = RGB( data.Int("R", i), data.Int("G", i), data.Int("B", i) );
    quals.push_back(item);
    qualmap[item.name] = item;
    debstr() << item.name << std::endl;
  }

  /* hacks to filter extra colors out.
  // std::vector< PRow > tmpRows;
  // tmpRows.push_back(row);
  ParseCSV filter = parser;
  filter.pobj.rows.clear();
  filter.pobj.rows = tmpRows;
  std::ofstream myOS("color2.csv");
  filter.store(myOS);
  */
}


QualItem& Qual::getItem(int ix) {
  assert(ix >= 0);
  static QualItem bad;
  if (quals.size() == 0) { return bad; }
  int modIx = ix % quals.size();
  QualItem& qa = quals[modIx];
  return qa;
}


Qual::~Qual()
{
}

Qual Qual::qual;
