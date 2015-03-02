#pragma once

#include <sstream>
#include <vector>

class LogEvents
{
public:
  LogEvents();
  ~LogEvents();

  static LogEvents log;

  int notifCounter; // is reset on each player cmd.  When > 0, user must be prompted.
  void resetNotif() { notifCounter = 0;  }
  void incNotif() { ++notifCounter;  }
  bool multiNotif() { return (notifCounter > 0);  }

  static void respectMultiNotif();

  std::vector< std::string > lines;
  void add(const std::string& s);
  void show();
};

class logstr : public std::stringstream { 
  public: 
  ~logstr(); 
  logstr() { pauseAfter = false;  }
  logstr(bool bAfter) { pauseAfter = bAfter; }
  bool pauseAfter;
};

class pauselog : public logstr {
public:
  pauselog() :logstr(true){}
};
