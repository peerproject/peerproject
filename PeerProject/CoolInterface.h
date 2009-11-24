//
// CoolInterface.h
//
// This file is part of PeerProject (peerproject.org) © 2008
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

#include "Skin.h"

//#define LVSIL_MID           50	// ToDo: 24x24 toolbar icons?
#define LVSIL_BIG             100	// 48x48 icons

class CCoolInterface
{
// Construction
public:
	CCoolInterface();
	virtual ~CCoolInterface();

public:
	CFont		m_fntNormal;		// system.plain
	CFont		m_fntBold;			// system.bold
	CFont		m_fntItalic;		// system.plain + italic
	CFont		m_fntBoldItalic;	// system.bold + italic
	CFont		m_fntUnder;			// system.plain + underline
	CFont		m_fntCaption;		// panel.caption
	CFont		m_fntNavBar;		// navbar.caption
	CFont		m_fntRichDefault;	// richdoc.default
	CFont		m_fntRichHeading;	// richdoc.heading

	void		Load();
	void		Clear();
	void		NameCommand(UINT nID, LPCTSTR pszName);
	UINT		NameToID(LPCTSTR pszName) const;
	int			ImageForID(UINT nID, int nImageListType = LVSIL_SMALL) const;
	void		AddIcon(UINT nID, HICON hIcon, int nImageListType = LVSIL_SMALL);
	void		CopyIcon(UINT nFromID, UINT nToID, int nImageListType = LVSIL_SMALL);
	HICON		ExtractIcon(UINT nID, BOOL bMirrored, int nImageListType = LVSIL_SMALL);
	int			ExtractIconID(UINT nID, BOOL bMirrored, int nImageListType = LVSIL_SMALL);
	// Set skinned icon to window: pWnd->SetIcon( hIcon, bBigIcon )
	void		SetIcon(UINT nID, BOOL bMirrored, BOOL bBigIcon, CWnd* pWnd);
	void		SetIcon(HICON hIcon, BOOL bMirrored, BOOL bBigIcon, CWnd* pWnd);
	int			GetImageCount(int nImageListType = LVSIL_SMALL);
	CImageList*	SetImageListTo(CListCtrl& pWnd, int nImageListType = LVSIL_SMALL);
	//BOOL		AddImagesFromToolbar(UINT nIDToolBar, COLORREF crBack = RGB(0,255,0));
	BOOL		Add(CSkin* pSkin, CXMLElement* pBase, HBITMAP hbmImage, COLORREF crMask, int nImageListType = LVSIL_SMALL);
	BOOL		Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle = ILD_NORMAL, int nImageListType = LVSIL_SMALL) const;
	BOOL		DrawEx(CDC* pDC, int nImage, POINT pt, SIZE sz = CSize( 16, 16 ), COLORREF clrBk = CLR_NONE, COLORREF clrFg = CLR_DEFAULT, UINT nStyle = ILD_NORMAL, int nImageListType = LVSIL_SMALL) const;
	BOOL		Draw(CDC* pDC, UINT nID, int nSize, int nX, int nY, COLORREF crBack = CLR_NONE, BOOL bSelected = FALSE, BOOL bExclude = TRUE) const;
	CDC*		GetBuffer(CDC& dcScreen, const CSize& szItem);
	BOOL		DrawWatermark(CDC* pDC, CRect* pRect, CBitmap* pMark, int nOffX = 0, int nOffY = 0);
	void		DrawThumbnail(CDC* pDC, const CRect& rcThumb, BOOL bWaiting, BOOL bSelected,
					CBitmap& bmThumb, int nIcon48 = -1, int nIcon32 = -1, const CString& strLabel = CString());
	void		CreateFonts(LPCTSTR pszFace = NULL, int nSize = 0);

	static BOOL	EnableTheme(CWnd* pWnd, BOOL bEnable = TRUE);

protected:
	typedef CMap< UINT, UINT, int, int > CUINTintMap;
	typedef CMap< CString, const CString&, UINT, UINT > CStringUINTMap;
	typedef CMap< HICON, HICON, HWND, HWND > CHICONHWNDMap;

	CStringUINTMap	m_pNameMap;
	CUINTintMap		m_pImageMap16;		// Small images (LVSIL_SMALL)
	CImageList		m_pImages16;		// Small images (LVSIL_SMALL)
	CUINTintMap		m_pImageMap32;		// Normal images (LVSIL_NORMAL)
	CImageList		m_pImages32;		// Normal images (LVSIL_NORMAL)
	CUINTintMap		m_pImageMap48;		// Normal images (LVSIL_BIG)
	CImageList		m_pImages48;		// Normal images (LVSIL_BIG)
	CSize			m_czBuffer;
	CDC				m_dcBuffer;
	CBitmap			m_bmBuffer;
	HBITMAP			m_bmOldBuffer;
	CHICONHWNDMap	m_pWindowIcons;

	BOOL			ConfirmImageList();
};

extern CCoolInterface CoolInterface;

typedef struct
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	WORD* items() { return (WORD*)(this+1); }
} TOOLBAR_RES;
