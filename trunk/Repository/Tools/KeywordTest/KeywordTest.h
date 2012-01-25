//
// KeywordTest.h
//
// This file is part of PeerProject (peerproject.org) © 2012
// Portions Copyright Shareaza Development Team, 2011.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "Resource.h"

class CKeywordTestApp : public CWinApp
{
public:
	CKeywordTestApp();

protected:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CKeywordTestApp theApp;
