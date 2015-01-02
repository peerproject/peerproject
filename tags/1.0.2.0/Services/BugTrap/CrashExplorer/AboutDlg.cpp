/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2009 IntelleSoft.
 * All rights reserved.
 *
 * Description: About dialog.
 * Author: Maksim Pyatkovskiy.
 */

#include "StdAfx.h"
#include "Resource.h"

#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @param hWnd - handle to the control to receive the default keyboard focus.
 * @param lParam - specifies additional initialization data.
 * @return message result code.
 */
LRESULT CAboutDlg::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	CenterWindow(GetParent());

	TCHAR szLinkURL[MAX_PATH];
	m_lblURL.SubclassWindow(GetDlgItem(IDC_URL));
	m_lblURL.GetWindowText(szLinkURL, countof(szLinkURL));
	m_lblURL.SetHyperLink(szLinkURL);
	return TRUE;
}
