#include "stdafx.h"
#include "ParseCSV.h"

#include <sstream>

ParseCSV::ParseCSV(std::istream& is_,ParseObj& pobj_)
:is(is_) //"color.csv")
, pobj(pobj_)
{
  clearBuf();
}


ParseCSV::~ParseCSV()
{
}

void ParseCSV::clearBuf() {
  // error C4996: 'strncpy': This function or variable may be unsafe. Consider using strncpy_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

  strncpy(buf, "", sizeof buf); // Clear/init buf.
  err = NULL;
}

char ParseCSV::delim = ';';




bool ParseCSV::parse() {

  if (!fileOK()) { err = "File bad already at outset of parse."; return false; }

  if (!parseTableName())   { return false; }
  if (!parseColumnNames()) { return false; }
  if (!parseRows())        { return false; }
  return true;
}

void ParseCSV::store(std::ostream& os) {
  //First line is tablename:
  os << pobj.tableName << std::endl;

  //Second line is columns:
  std::vector< std::string >::iterator i;
  for (i = pobj.columns.begin(); i != pobj.columns.end(); ++i) {
    std::string column = *i;
    if (i != pobj.columns.begin()) {
      os << delim;
    }
    os << column;
  }
  os << std::endl;

  for (int i = 0; i < pobj.size(); ++i) {
    PRow& row = pobj[i];
    PRow::iterator j; 
    for (j = row.begin(); j != row.end(); ++j) {
      std::string field = *j;
      if (j != row.begin()) {
        os << delim;
      }
      os << field;
    }
    os << std::endl;
  }
}


bool ParseCSV::parseTableName() {
  // Get table name from first line:
  clearBuf();
  if (!is.getline(buf, sizeof buf)) { err = "file (table) returns empty."; return false; }
  if (!is.good() || is.bad()) { err = "bad tableName, first line."; return false; }
  pobj.tableName = buf;
  return true;
}

bool ParseCSV::parseColumnNames() {
  // Get column names from second line:
  clearBuf();
  is.getline(buf, sizeof buf);
  if (!is.good() || is.bad()) { err = "bad column names."; return false; }

  std::stringstream ss(buf);
  std::string column;
  int count = 0;
  while (std::getline(ss, column, delim)) {
    pobj.columns.push_back(column);
    pobj.column_map[column] = count++;
  }
  return true;
}


bool ParseCSV::parseRows() {
  // get rows from rest:
  clearBuf();
  while (is.getline(buf, sizeof buf)) {
    if (!is.good() || is.bad()) { err = "bad column names."; return false; }
    parseRow();
  }
  return true;
}


bool ParseCSV::parseRow() { // row is already in buf
  std::stringstream ss(buf);

  pobj.rows.push_back(PRow());
  PRow& row = pobj.rows.back();

  std::string field;
  while (std::getline(ss, field, delim)) {
    row.push_back(field);
  }
  return true;
}



std::string& ParseObj::field(const char* column, int ix) {
  PRow& row = rows[ix];
  int col = column_map[column];
  if (col >= (int) row.size()) { row.resize(col + 1); }
  return row[col];
}

int ParseObj::Int(const char* column, int ix) {
  std::string& s = field(column, ix);
  std::stringstream ss(s);
  int i = 0; ss >> i;
  return i;
}