#pragma once

#include <ostream>

class Cmd {
public:
  virtual bool legal(std::ostream& err);
  virtual bool Do(std::ostream& err);
  virtual bool silent() const;
};

