//
// Skin.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
// Portions copyright Shareaza Development Team, 2002-2008.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "CoolMenu.h"

class CCoolBarCtrl;
class CXMLElement;
class CSkinWindow;


class CSkin
{
// Construction
public:
	CSkin();
	virtual ~CSkin();

// Attributes
public:
	static BOOL	m_bSkinChanging;	// True only during CMainWnd::OnSkinChanged process

// Operations
public:
	void		Apply();
	void		Clear();
	BOOL		LoadFromFile(LPCTSTR pszFile);
	BOOL		LoadFromResource(HINSTANCE hInstance, UINT nResourceID);
	BOOL		LoadFromString(const CString& strXML, const CString& strPath);
	BOOL		LoadFromXML(CXMLElement* pXML, const CString& strPath);
	BOOL		SelectCaption(CWnd* pWnd, int nIndex);
	BOOL		SelectCaption(CString& strCaption, int nIndex);
	void		DrawWrappedText(CDC* pDC, CRect* pBox, LPCTSTR pszText, CPoint ptStart, BOOL bExclude = TRUE);
protected:
	void		ApplyRecursive(LPCTSTR pszPath);
	void		CreateDefault();
	void		CreateDefaultColors();
	HBITMAP		LoadBitmap(const CString& strName);

// Strings
public:
	void		AddString(const CString& strString, UINT nStringID);
	BOOL		LoadString(CString& str, UINT nStringID) const;
	BOOL		LoadControlTip(CString& str, UINT nCtrlID);
	int			GetTextFlowChange(LPCTSTR pszText, BOOL* bIsRTL);
protected:
	BOOL		LoadStrings(CXMLElement* pBase);
	BOOL		LoadControlTips(CXMLElement* pBase);
	CMap<UINT, UINT, CString, const CString&>	m_pStrings;
	CMap<UINT, UINT, CString, const CString&>	m_pControlTips;

// Menus
public:
	CMenu*		GetMenu(LPCTSTR pszName) const;
	CMenu*		CreatePopupMenu(LPCTSTR pszName);
	void		TrackPopupMenu(LPCTSTR pszMenu, const CPoint& point, UINT nDefaultID = 0, const CStringList& oFiles = CStringList(), CWnd* pWnd = AfxGetMainWnd()) const;
protected:
	CMap< CString, const CString&, CMenu*, CMenu* > m_pMenus;
	BOOL		LoadMenus(CXMLElement* pBase);
	BOOL		LoadMenu(CXMLElement* pXML);
	BOOL		CreateMenu(CXMLElement* pXML, HMENU hMenu);

// Toolbars
public:
	BOOL		CreateToolBar(LPCTSTR pszName, CCoolBarCtrl* pBar);
	CCoolBarCtrl* CreateToolBar(LPCTSTR pszName);
	CCoolBarCtrl* GetToolBar(LPCTSTR pszName) const;
protected:
	CMap< CString, const CString&, CCoolBarCtrl*, CCoolBarCtrl* > m_pToolbars;
	BOOL		LoadToolbars(CXMLElement* pBase);
	BOOL		CreateToolBar(CXMLElement* pElement);

// Documents
public:
	CXMLElement* GetDocument(LPCTSTR pszName);
protected:
	BOOL		LoadDocuments(CXMLElement* pBase);
	CMap< CString, const CString&, CXMLElement*, CXMLElement* > m_pDocuments;

// Watermarks
public:
	HBITMAP		GetWatermark(LPCTSTR pszName);
	BOOL		GetWatermark(CBitmap* pBitmap, LPCTSTR pszName);
protected:
	BOOL		LoadWatermarks(CXMLElement* pSub, const CString& strPath);
	CMap< CString, const CString&, CString, CString& > m_pWatermarks;

// Translate
public:
	BOOL		Translate(LPCTSTR pszName, CHeaderCtrl* pCtrl);
	CString		GetHeaderTranslation(LPCTSTR pszClassName, LPCTSTR pszHeaderName);
protected:
	BOOL		LoadListColumns(CXMLElement* pBase);
	CMap< CString, const CString&, CString, CString& > m_pLists;

// Dialogs
public:
	BOOL		Apply(LPCTSTR pszName, CDialog* pDialog, UINT nIconID = 0, CToolTipCtrl* pWndTooltips = NULL);
	CString	GetDialogCaption(LPCTSTR pszName);
protected:
	BOOL		LoadDialogs(CXMLElement* pBase);
	CMap< CString, const CString&, CXMLElement*, CXMLElement* >	m_pDialogs;

// Window Skins
public:
	CSkinWindow* GetWindowSkin(LPCTSTR pszWindow, LPCTSTR pszAppend = NULL);
	CSkinWindow* GetWindowSkin(CWnd* pWnd);
protected:
	BOOL		LoadWindowSkins(CXMLElement* pSub, const CString& strPath);
	CList< CSkinWindow* > m_pSkins;

// Watermarks + Color Scheme (CColors/CImages)
public:
	CBitmap		m_bmBanner;
	int 		m_nBanner;

	CBrush		m_brDialog;
	CBrush		m_brDialogPanel;
	CBrush		m_brMediaSlider;

	CBitmap		m_bmDialog;
	CBitmap		m_bmDialogPanel;
	CBitmap		m_bmPanelMark;

	CBitmap		m_bmSelected;
	CBitmap		m_bmToolTip;

protected:
	BOOL		LoadColorScheme(CXMLElement* pBase);

// Fonts
protected:
	CList< CString > m_pFontPaths;
protected:
	BOOL		LoadFonts(CXMLElement* pBase, const CString& strPath);

// Other
public:
	UINT		LookupCommandID(CXMLElement* pXML, LPCTSTR pszName = _T("id")) const;
	CString 	GetImagePath(UINT nImageID) const;
protected:
	CMap< UINT, const UINT&, CString, const CString& > m_pImages;
	BOOL		LoadResourceMap(CXMLElement* pBase);
	BOOL		LoadCommandImages(CXMLElement* pBase, const CString& strPath);
	BOOL		LoadCommandIcon(CXMLElement* pXML, const CString& strPath);
	BOOL		LoadCommandBitmap(CXMLElement* pBase, const CString& strPath);

// Mode Suffixes
protected:
	static LPCTSTR m_pszModeSuffix[3][4];

// NavBar
public:
	CRect		m_rcNavBarOffset;
	enum { NavBarNormal, NavBarUpper, NavBarLower } m_NavBarMode;
protected:
	BOOL		LoadNavBar(CXMLElement* pBase);
	BOOL		LoadOptions(CXMLElement* pBase);

// Option Settings
public:
	BOOL		m_bDropMenu;
	BOOL		m_bMenuBorders;
	BOOL		m_bMenuGripper;
	BOOL		m_bRoundedSelect;
	int			m_nButtonEdge;
	int			m_nToolbarHeight;
	int			m_nTaskbarHeight;
	int			m_nTaskbarTabWidth;
	int			m_nGroupsbarHeight;
	int			m_nHeaderbarHeight;
	int			m_nMonitorbarWidth;
	int			m_nSidebarWidth;
	int			m_nSidebarPadding;
	int			m_nSplitter;
	int 		m_nLibIconsX;
	int 		m_nLibIconsY;

private:
	CSkin(const CSkin&);
	CSkin& operator=(const CSkin&);
};


extern CSkin Skin;
