//
// SkinWindow.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions copyright Shareaza Development Team, 2002-2007.
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

class CXMLElement;


class CSkinWindow
{
public:
	CSkinWindow();
	~CSkinWindow();

public:
	typedef CMap< CString, const CString&, CRect*, CRect* > CRectMap;

	CDC			m_dcSkin;
	CBitmap		m_bmSkin;
	CBitmap		m_bmWatermark;
	CRectMap	m_pAnchorList;	// Typedef above
	CString		m_sTargets;
	CString		m_sLanguage;
	CFont		m_fnCaption;
	COLORREF	m_crCaptionText;
	COLORREF	m_crCaptionInactive;
	COLORREF	m_crCaptionShadow;
	COLORREF	m_crCaptionOutline;

	BOOL		Parse(CXMLElement* pXML, const CString& strPath);
	void		Prepare(CDC* pDC);
	void		CalcWindowRect(RECT* pRect, BOOL bToClient = FALSE, BOOL bZoomed = FALSE);
	BOOL		GetPart(LPCTSTR pszName, CRect& rcPart);
	BOOL		GetAnchor(LPCTSTR pszName, CRect& rcAnchor);
	BOOL		GetAnchor(LPCTSTR pszName, const CRect& rcClient, CRect& rcAnchor);
	BOOL		PaintPartOnAnchor(CDC* pDC, const CRect& rcClient, LPCTSTR pszPart, LPCTSTR pszAnchor);
	BOOL		PreBlend(CBitmap* pbmTarget, const CRect& rcTarget, const CRect& rcSource);
	void		OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	void		OnSetText(CWnd* pWnd);
	void		OnSize(CWnd* pWnd);
	BOOL		OnEraseBkgnd(CWnd* pWnd, CDC* pDC);
	void		OnNcCalcSize(CWnd* pWnd, BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	UINT		OnNcHitTest(CWnd* pWnd, CPoint point, BOOL bResizable = FALSE);
	BOOL		OnNcActivate(CWnd* pWnd, BOOL bActive);
	void		OnNcPaint(CWnd* pWnd);
	void		OnNcMouseMove(CWnd* pWnd, UINT nHitTest, CPoint point);
	BOOL		OnNcLButtonDown(CWnd* pWnd, UINT nHitTest, CPoint point);
	BOOL		OnNcLButtonUp(CWnd* pWnd, UINT nHitTest, CPoint point);
	BOOL		OnNcLButtonDblClk(CWnd* pWnd, UINT nHitTest, CPoint point);

protected:
	HBITMAP		m_hoSkin;
	CBitmap		m_bmAlpha;
	CRectMap	m_pPartList;	// Typedef above
	BOOL*		m_bPart;
	int*		m_nPart;
	CRect*		m_rcPart;
	CRect*		m_rcAnchor;
	BOOL*		m_bAnchor;
	CSize		m_szMinSize;
	CRect		m_rcMaximise;
	CRect		m_rcResize;
	BOOL		m_bCaption;
	BOOL		m_bCaptionCaps;
	CRect		m_rcCaption;
	int			m_nCaptionAlign;
	int			m_nHoverAnchor;
	int			m_nDownAnchor;
	int			m_nMirror;
	CRect		m_rcMirror;
	CXMLElement* m_pRegionXML;

	CSize		GetRegionSize();
	void		Paint(CWnd* pWnd, TRISTATE bActive = TRI_UNKNOWN);
	BOOL		ParseRect(CXMLElement* pXML, CRect* pRect);
	BOOL		ParseColor(const CString& str, COLORREF& cr);
	void		ResolveAnchor(const CRect& rcClient, CRect& rcAnchor, int nAnchor);
	void		SelectRegion(CWnd* pWnd);
};

enum
{
	SKINPART_TOP_LEFT, SKINPART_TOP, SKINPART_TOP_RIGHT,
	SKINPART_LEFT_TOP, SKINPART_LEFT, SKINPART_LEFT_BOTTOM,
	SKINPART_RIGHT_TOP, SKINPART_RIGHT, SKINPART_RIGHT_BOTTOM,
	SKINPART_BOTTOM_LEFT, SKINPART_BOTTOM, SKINPART_BOTTOM_RIGHT,
	SKINPART_TOP_LEFT_ALT, SKINPART_TOP_ALT, SKINPART_TOP_RIGHT_ALT,
	SKINPART_LEFT_TOP_ALT, SKINPART_LEFT_ALT, SKINPART_LEFT_BOTTOM_ALT,
	SKINPART_RIGHT_TOP_ALT, SKINPART_RIGHT_ALT, SKINPART_RIGHT_BOTTOM_ALT,
	SKINPART_BOTTOM_LEFT_ALT, SKINPART_BOTTOM_ALT, SKINPART_BOTTOM_RIGHT_ALT,

	SKINPART_SYSTEM, SKINPART_SYSTEM_HOT, SKINPART_SYSTEM_DOWN,
	SKINPART_MINIMISE, SKINPART_MINIMISE_HOT, SKINPART_MINIMISE_DOWN,
	SKINPART_MAXIMISE, SKINPART_MAXIMISE_HOT, SKINPART_MAXIMISE_DOWN,
	SKINPART_CLOSE, SKINPART_CLOSE_HOT, SKINPART_CLOSE_DOWN,

	SKINPART_COUNT
};

enum
{
	SKINANCHOR_ICON, SKINANCHOR_SYSTEM,
	SKINANCHOR_MINIMISE, SKINANCHOR_MAXIMISE, SKINANCHOR_CLOSE,

	SKINANCHOR_COUNT
};

enum
{
	SKINPARTMODE_TILE, SKINPARTMODE_STRETCH
};
