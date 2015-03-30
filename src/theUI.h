#ifndef THE_UI_H
#define THE_UI_H

class TheUI { // namespace for UI functions..
public:
  static unsigned int getNextKey(const char* file, int line, const char* reason);
  static int waitForKey(const char* file, int line, const char* reason); // silently pause until user presses some key.
  static int promptForAnyKey(const char* file, int line, const char* reason); // ("auto-")PROMPT user to press any key.
  static int promptForKey(const char* prompt, const char* file, int line, const char* reason); // Specifically prompt user to press keys, ie with a description.

  static int microSleepForRedraw(int maxCount); // allow redraw

  static void invalidateCellXY(int tx, int ty); // Interface
  static void invalidateCell(CPoint tilepos);   // Interface

  static void invalidateVPCell(CPoint vp);   // Interface  viewport
  static void invalidateTPCell(CPoint vp);   // Interface term

  static void invalidateWndJG(CRect* pRect, bool erase); // Used for impl.
  static bool shiftKey();
  static bool ctrlKey();
  static void BeepWarn();

  static bool hasQuit;
};

#endif // THE_UI_H
