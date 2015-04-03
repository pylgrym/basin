#include "stdafx.h"
#include "LogEvents.h"
#include "Cuss.h"
#include "theUI.h"
#include "util/debstr.h"
#include "cellmap/cellmap.h"
#include "Term.h"
#include "FightDashboard.h"



LogEvents::LogEvents()
{
}


LogEvents::~LogEvents()
{
}

LogEvents LogEvents::log;


void LogEvents::respectMultiNotif() {
  debstr() << "respectMultiNotif, counter:" << log.notifCounter << std::endl;

  if (log.multiNotif()) {
    Cuss::move(CPoint(Term::Width-5, 0)); // was: Map::Width  (bug)
    Cuss::prt("MORE",true);
    TheUI::waitForKey(__FILE__, __LINE__, "more-pause");
  }
}


enum HitCase { HC_RemHP=0, HC_LastHit=1,HC_MaxHP = 2};

HitCase LogEvents_remainingHP(int ix, int maxIx) { // , int lastHit) {
  FightDashboard& board = FightDashboard::dashboard;
  // board.hp, board.maxHP, board.lastHit
  // ratio board.hp/board.maxHP  must match 
  // ratio ix/maxIx = hp/maxHP
  // IE  ix = maxIx*hp/maxHP
  int maxHP_adj = board.maxHP;
  if (maxHP_adj == 0) { maxHP_adj = 1;  }

  double thres = (double)maxIx*board.hp / (double) maxHP_adj;
  int intThres = int(thres + 0.5);

  if (ix <= intThres) { return HC_RemHP; } // If so low, you are within the remaining-HP.
  int deltaIx = (ix - intThres);

  double LH_thres = (double)maxIx*board.lastHit / (double) maxHP_adj; // board.maxHP;
  int int_LHThres = int(LH_thres + 0.5);
  if (deltaIx <= int_LHThres) { return HC_LastHit; }

  return HC_MaxHP;
}

void LogEvents::prtL_bar(const std::string& s) {
  // NB!, this entire approach is kludgey, and meant as experiment.
  bool bClipped = false;
  for (int i = 0; i < (int) s.length() || !bClipped; ++i) {
    HitCase remHP = LogEvents_remainingHP(i, 40); // 48);
    COLORREF color = (remHP == HC_RemHP ? RGB(0, 128, 0) : RGB(128, 128, 128));
    if (remHP == HC_LastHit) { color = RGB(255, 0, 0); } // red.


    PushFgCol fg(RGB(245, 245, 255)); // Cuss::setTxtColor(RGB(245, 245, 255)); // color); // should be white/remove.
    PushBkCol bg(color); // Cuss::setBkColor(color); // should remain when impl correctly.

    char c = ' ';
    if (i < (int)s.length()) {
      c = s[i];
    }
    bClipped = Cuss::putchar(c, true);
  }

  // while (!bClipped) { // clear rest of line.
  //   bClipped = putchar(' ', true);
  //}

  // reset background when finished:
  //Cuss::setBkColor(RGB(0, 0, 32)); // color); // should remain when impl correctly.
 }

void LogEvents::add(const std::string& s) {
  lines.push_back(s);

  respectMultiNotif(); // " - MORE" pause if several messages..

  incNotif();
  Cuss::move(CPoint(0,0));

  bool classicWay = false;
  if (classicWay) {
    Cuss::prtL(s.c_str()); // todo - color bar here.
  } else {
    prtL_bar(s);
  }

  /* JG; fixme/todo: instead of just printing this,
  we need some 'feeding, breaking down msgs' for the user..
  If we print a small info that fits,
  it's not a problem. But if we have to tell the
  user several things, we need to break it up and feed it piece-wise to him,
  with <more> keyboard-prompts. Piecemeal.
    Presumably we need to count them, and reset count on each command,
    and do prompt if count is > 0?
  */
}





logstr::~logstr() {
	std::string s = str();
  if (s.empty()) { return;  } // Don't log empty stuff.

  CA2T uc(s.c_str(), CP_ACP);
  // OutputDebugString(uc);

  LogEvents::log.add(s);

  if (pauseAfter) { // used, if you know you want to display a different screen after the msg 
    //              // (the screen is not a logmsg, but it means you must more-pause first, to let the msg be read.)
    LogEvents::respectMultiNotif(); // will show 'more', based on log.add putting us in 'multi-mode'.
    LogEvents::log.resetNotif();    // Will clean up the multi-mode, so we avoid double-triggering the effect. (because the 'after' mode should display immediately, and not itself trigger a 'prev-more'.)
  }

}



void LogEvents::show() {
  
  Cuss::clear(false);
    
  std::vector< std::string >::iterator i;
  i = lines.begin();

  if (lines.size() > Term::Height) { // Viewport::VP_Height) {
    i = (lines.end() - Term::Height); // Viewport::VP_Height);
  }

  for (; i != lines.end(); ++i) {
    const std::string& s = *i;
    Cuss::prtL(s.c_str());  
  }

  TheUI::promptForAnyKey(__FILE__, __LINE__, "log-pause");
  Cuss::clear(true);
}
