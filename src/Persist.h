#pragma once

#include <ostream>
//#include <istream>
#include <fstream>

class Persist
{
public:
  bool bOut;
  std::ostream& os;
  std::ifstream is;
  Persist(std::ostream& os_);
  ~Persist();

  /* template/traits approach could do this.

  figure out design for objdesc instead of obj type.
  */
  bool transfer(int& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    } else {
      is >> i;
    }
    return true;
  }

  bool transfer(long& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    }
    else {
      is >> i;
    }
    return true;
  }

  bool transfer(double& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    }
    else {
      is >> i;
    }
    return true;
  }
};

