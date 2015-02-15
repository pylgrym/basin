
// ChildView.h : interface of the CChildView class
//


#pragma once

#include "cellmap/cellmap.h"
#include "sprites/Tilemap.h"
#include "Mob.h"

// CChildView window

const int timerID = 1001;

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
  Tiles tiles;
  //MobQueue queue;

private:
  static CChildView* singletonWnd;

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
  // afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

  friend class TheUI;
};

