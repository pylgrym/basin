#include "stdafx.h"
#include "Cmd.h"

// #include "util/debstr.h"
#include "LogEvents.h"

bool Cmd::legal(std::ostream& err) { 
  return true;  
}

bool Cmd::Do(std::ostream& err) {  
  logstr err2; // debstr
  std::stringstream silence;
  if (!legal(silent() ? silence : err2)) {
    // if (!silent()) { err2 << "\n";  }
    return false;
  }
  return true;
}

bool Cmd::silent() const { 
  return true;  
}
