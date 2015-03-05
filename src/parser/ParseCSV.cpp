#include "stdafx.h"
#include "ParseCSV.h"

#include <sstream>

ParseCSV::ParseCSV(std::istream& is_)
:is(is_) //"color.csv")
{
  clearBuf();
}


ParseCSV::~ParseCSV()
{
}

void ParseCSV::clearBuf() {
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
