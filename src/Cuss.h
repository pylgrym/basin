#pragma once


struct Cuss {
  static CPoint csr;

  static void setCellBg(CPoint p, COLORREF color);
  static void setCellFg(CPoint p, COLORREF color);

  static COLORREF curTxtColor;
  static void setTxtColor(COLORREF tcol = RGB(255, 255, 255) );

  static void setBkColor(COLORREF tcol); // { curBkColor = tcol; }
  static COLORREF curBkColor; // = RGB(0, 0, 64); // 255, 255, 255);

  static void clear(bool bInvalidate);
  static void clearLine(int line, bool bInvalidate);
  static bool prt(const char* txt, bool bClip); // Print string at cursor, leaving the cursor at last char. Returns true if clipped.
  static bool prtCR(const char* txt, bool bClip); // Print string, then do CRLF. Returns true if clipped.
  static void prtL(const char* txt, bool stay=false); // Print/clear full line.
  static bool putchar(char c, bool bClip);
  static void move(CPoint pos);

  static void CRLF(); // Do carriage-return + lineFeed.

  static void invalidate();
};


class PushBkCol {
public:
  COLORREF oldCol;
  PushBkCol(COLORREF newCol) {
    oldCol = Cuss::curBkColor;
    Cuss::setBkColor(newCol);
  }

  ~PushBkCol() {
    Cuss::setBkColor(oldCol);
  }
};

class PushFgCol {
public:
  COLORREF oldCol;
  PushFgCol(COLORREF newCol) {
    oldCol = Cuss::curTxtColor;
    Cuss::setTxtColor(newCol);
  }

  ~PushFgCol() {
    Cuss::setTxtColor(oldCol);
  }
};
