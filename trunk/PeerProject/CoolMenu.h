//
// CoolMenu.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#pragma once

#include <shlobj.h>

struct __declspec(uuid("000214f4-0000-0000-c000-000000000046")) IContextMenu2;
struct __declspec(uuid("bcfce0a0-ec17-11d0-8d10-00a0c90f2719")) IContextMenu3;

#define ID_SHELL_MENU_MIN 40000
#define ID_SHELL_MENU_MAX 41000

class CCoolMenu
{
// Construction
public:
	CCoolMenu();
	virtual ~CCoolMenu();

// Operations
public:
	void		Clear();
	BOOL		AddMenu(CMenu* pMenu, BOOL bChild = FALSE);
	BOOL		ReplaceMenuText(CMenu* pMenu, int nPosition, MENUITEMINFO FAR* mii, LPCTSTR pszText);
	void		SetWatermark(HBITMAP hBitmap);
	void		OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	void		OnMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void		OnDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	LRESULT		OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	UINT_PTR	DoExplorerMenu(HWND hwnd, const CStringList& oFiles, POINT point, HMENU hMenu, HMENU hSubMenu, UINT nFlags);

protected:
	CComPtr< IContextMenu >		m_pContextMenuCache;
	CComPtr< IContextMenu2 >	m_pContextMenu2;
	CComPtr< IContextMenu3 >	m_pContextMenu3;

	void		OnMeasureItemInternal(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void		OnDrawItemInternal(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void		DrawMenuText(CDC* pDC, CRect* pRect, const CString& strText);
	void		DrawWatermark(CDC* pDC, CRect* pRect, int nOffX, int nOffY);
	void		SetSelectmark(HBITMAP hBitmap);

// Attributes
protected:
	// Note: CMap< DWORD_PTR, DWORD_PTR, ... causes a conversion to DWORD within CMap
	// DWORD_PTR& seems to solve the problem - should be investigated (TODO)
	// my guess: the compiler cannot distinguish between T and __w64 T with respect to overload resolution
	//   or template instantiation - in that case it's a false warning and should be supressed
	CMap< DWORD_PTR, DWORD_PTR&, CString, CString& >	m_pStrings;
protected:
	BOOL		m_bSelectTest;
	CBitmap		m_bmSelectmark;
	CBitmap		m_bmWatermark;
	CDC			m_dcWatermark;
	CSize		m_czWatermark;
	HBITMAP		m_hOldMark;
protected:
	BOOL		m_bEnable;
	BOOL		m_bUnhook;
	CString		m_sFilterString;
	CString		m_sOldFilterString;

// Border Hook
public:
	void			EnableHook();
	static void		EnableHook(BOOL bEnable);
	static void		RegisterEdge(int nLeft, int nTop, int nLength);
	static BOOL		IsModernVersion();
protected:
	static LRESULT	CALLBACK MsgHook(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT	CALLBACK MenuProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	static HHOOK	m_hMsgHook;
	static LPCTSTR	wpnOldProc;
	static BOOL		m_bPrinted;
	static int		m_nEdgeLeft;
	static int		m_nEdgeTop;
	static int		m_nEdgeSize;

private:
	CCoolMenu(const CCoolMenu&);
	CCoolMenu& operator=(const CCoolMenu&);
};

extern CCoolMenu CoolMenu;
