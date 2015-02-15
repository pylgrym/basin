
// Basin.h : main header file for the Basin application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CBasinApp:
// See Basin.cpp for the implementation of this class
//

class CBasinApp : public CWinApp
{
public:
	CBasinApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBasinApp theApp;
