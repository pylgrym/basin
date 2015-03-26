#pragma once

#include "util/debstr.h"

class TimeMeasure {
public:
  int start, end;
  std::string what;
  bool autoLog;

  TimeMeasure(const char* what_, bool autoLog_)
  :autoLog(autoLog_)
  {
    start = end = 0;
    what = what_;
    startClock();
  }

  void startClock() {
    start = measure(); // find way to measure time from sys. (nb, GetSystemTime is  not very accurate.)
  }

  int stopClock() {
    end = measure();
    return getDelta();
  }

  ~TimeMeasure() {
    stopClock();
    if (autoLog) { showTime(); }
  }

  int getDelta() {
    int delta = (end - start);
    return delta;
  }

  void showTime() {
    int delta = (end - start);
    debstr() << "time ms:" << delta << ", " << what << "\n";
  }


  int measure() { return readTime();  }

  static int readTime() {
    SYSTEMTIME st;
    GetSystemTime(&st);
    int sumMS =
      ((st.wHour * 60 + st.wMinute) * 60 + st.wSecond) * 1000 + st.wMilliseconds;
    return sumMS;
  }

};

