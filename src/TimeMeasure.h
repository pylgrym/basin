#pragma once

#include "util/debstr.h"

class TimeMeasure {
public:
  int start, end;
  std::string what;

  TimeMeasure(const char* what_) {
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
    showTime();
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

