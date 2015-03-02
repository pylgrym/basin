#include "stdafx.h"
#include "LogEvents.h"

#include "Cuss.h"

#include "theUI.h"

#include "util/debstr.h"

#include "cellmap/cellmap.h"

LogEvents::LogEvents()
{
}


LogEvents::~LogEvents()
{
}

LogEvents LogEvents::log;


void LogEvents::respectMultiNotif() {
  if (log.multiNotif()) {
    Cuss::move(CPoint(Map::Width-5,0));
    Cuss::prt("MORE",true);
    TheUI::waitForKey(__FILE__, __LINE__, "more-pause");
  }
}

void LogEvents::add(const std::string& s) {
  lines.push_back(s);

  respectMultiNotif(); // " - MORE" pause if several messages..

  incNotif();
  Cuss::move(CPoint(0,0));
  Cuss::prtL(s.c_str());
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

  LogEvents::log.add(s);

	CA2T uc(s.c_str(), CP_ACP);  
	OutputDebugString(uc);
}



void LogEvents::show() {
  
  Cuss::clear(false);
    
  std::vector< std::string >::iterator i;
  i = lines.begin();

  if (lines.size() > Map::Height) {
    i = (lines.end() - Map::Height);
  }

  for (; i != lines.end(); ++i) {
    const std::string& s = *i;
    Cuss::prtL(s.c_str());  
  }

  TheUI::promptForAnyKey(__FILE__, __LINE__, "log-pause");
  Cuss::clear(true);
}
