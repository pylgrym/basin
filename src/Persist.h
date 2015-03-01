#pragma once

#include <ostream>
//#include <istream>
#include <fstream>
#include <string>

#include <sstream>
#include "util/debstr.h"

class Persist
{
public:
  bool bOut;

  std::stringstream dummy_;
  std::ifstream dummy2;
  std::ostream& os;
  std::istream& is;
  Persist(std::ostream& os_);
  Persist(std::istream& is_);
  ~Persist();

  /* template/traits approach could do this.

  figure out design for objdesc instead of obj type.
  */
  void eatLabel(std::istream& is, const std::string& expected);

  bool transfer(int& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    } else { eatLabel(is,label);
      is >> i;
    }
    return true;
  }

  bool transfer(long& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    } else { eatLabel(is, label);
      is >> i;
    }
    return true;
  }

  bool transfer(bool& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    } else { eatLabel(is, label);
      is >> i;
    }
    return true;
  }

  bool transfer(double& i, const char* label) {
    if (bOut) {
      os << label << " " << i << "\n";
    } else { eatLabel(is, label);
      is >> i;
    }
    return true;
  }
};

