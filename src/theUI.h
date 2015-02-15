#ifndef THE_UI_H
#define THE_UI_H

class TheUI { // namespace for UI functions..
public:
  static unsigned int getNextKey();
  static void waitForKey(); // silently pause until user presses some key.
  static void promptForAnyKey(); // ("auto-")PROMPT user to press any key.
  static int promptForKey(const char* prompt); // Specifically prompt user to press keys, ie with a description.

  static int microSleepForRedraw(int maxCount); // allow redraw

  static void invalidateCellXY(int tx, int ty); // Interface
  static void invalidateCell(CPoint tilepos);   // Interface
  static void invalidateWndJG(CRect* pRect, bool erase); // Used for impl.
  static bool shiftKey();
  static bool ctrlKey();
  static void BeepWarn();

};

#endif // THE_UI_H
