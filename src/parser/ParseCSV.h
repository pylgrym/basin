#pragma once

#include <string>
#include <vector>
#include <map>

typedef std::vector< std::string> PRow;

struct ParseObj {
  std::string tableName;
  std::vector< std::string > columns;
  std::map< std::string, int > column_map;
  std::vector< PRow > rows;

  int size() const { return rows.size();  }
  PRow& operator [] (int ix) { return rows[ix];  }

  std::string& field(const char* column, int ix);
  int            Int(const char* column, int ix); // same as 'field', but returns int.
};
// Error	1	error C4996 : 'strncpy' : This function or variable may be unsafe.Consider using strncpy_s instead.To disable deprecation, use _CRT_SECURE_NO_WARNINGS.See online help for details.d : \sandbox\grit\grit\parsecsv.cpp	18	1	grit


class ParseCSV
{
public:
  ParseCSV(std::istream& is_, ParseObj& pobj_);
  ~ParseCSV();


  ParseObj& pobj;
  std::istream& is; // std::ifstream is;

  const char* err;
  char buf[4096];
  bool parse();
  bool parseTableName();
  bool parseColumnNames();
  bool parseRows();
  bool parseRow();



  void clearBuf();
  bool fileOK() { return is.good() && !is.bad(); }

  static char delim; 
};

