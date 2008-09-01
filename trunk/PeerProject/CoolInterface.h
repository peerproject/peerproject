//
// CoolInterface.h
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

#define LVSIL_BIG             100	// 48x48 icons

#include "Skin.h"

class CCoolInterface
{
// Construction
public:
	CCoolInterface();
	virtual ~CCoolInterface();

public:
	CFont		m_fntNormal;		// system.plain
	CFont		m_fntItalic;		// system.plain + italic
	CFont		m_fntUnder;			// system.plain + underline
	CFont		m_fntBold;			// system.bold
	CFont		m_fntBoldItalic;	// system.bold + italic
	CFont		m_fntCaption;		// panel.caption
	CFont		m_fntNavBar;		// navbar.caption
	BOOL		m_bCustom;

	COLORREF	m_crWindow;
	COLORREF	m_crMidtone;
	COLORREF	m_crText;
	COLORREF	m_crHiText;
	COLORREF	m_crHiBorder;
	COLORREF	m_crHighlight;
	COLORREF	m_crBackNormal;
	COLORREF	m_crBackSel;
	COLORREF	m_crBackCheck;
	COLORREF	m_crBackCheckSel;
	COLORREF	m_crMargin;
	COLORREF	m_crBorder;
	COLORREF	m_crShadow;
	COLORREF	m_crCmdText;
	COLORREF	m_crCmdTextSel;
	COLORREF	m_crDisabled;

	COLORREF	m_crTipBack;
	COLORREF	m_crTipText;
	COLORREF	m_crTipBorder;
	COLORREF	m_crTipWarnings; // Color of warning messages

	COLORREF	m_crTaskPanelBack;

	COLORREF	m_crTaskBoxCaptionBack;
	COLORREF	m_crTaskBoxPrimaryBack;
	COLORREF	m_crTaskBoxCaptionText;
	COLORREF	m_crTaskBoxPrimaryText;
	COLORREF	m_crTaskBoxCaptionHover;
	COLORREF	m_crTaskBoxClient;

	COLORREF	m_crMediaWindowBack;
	COLORREF	m_crMediaWindowText;
	COLORREF	m_crMediaStatusBack;
	COLORREF	m_crMediaStatusText;
	COLORREF	m_crMediaPanelBack;
	COLORREF	m_crMediaPanelText;
	COLORREF	m_crMediaPanelActiveBack;
	COLORREF	m_crMediaPanelActiveText;
	COLORREF	m_crMediaPanelCaptionBack;
	COLORREF	m_crMediaPanelCaptionText;

	COLORREF	m_crTrafficWindowBack;
	COLORREF	m_crTrafficWindowText;
	COLORREF	m_crTrafficWindowGrid;

	COLORREF	m_crMonitorHistoryBack;
	COLORREF	m_crMonitorHistoryBackMax;
	COLORREF	m_crMonitorHistoryText;
	COLORREF	m_crMonitorDownloadLine;
	COLORREF	m_crMonitorUploadLine;
	COLORREF	m_crMonitorDownloadBar;
	COLORREF	m_crMonitorUploadBar;

	COLORREF	m_crRatingNull;
	COLORREF	m_crRating0;
 	COLORREF	m_crRating1;
 	COLORREF	m_crRating2;
 	COLORREF	m_crRating3;
 	COLORREF	m_crRating4;
 	COLORREF	m_crRating5;

 	COLORREF	m_crRichdocBack;
 	COLORREF	m_crRichdocText;
	COLORREF	m_crRichdocHeading;
	COLORREF	m_crTextAlert;
	COLORREF	m_crTextStatus;
	COLORREF	m_crTextLink;
 	COLORREF	m_crTextLinkHot;

 	COLORREF	m_crChatIn;
	COLORREF	m_crChatOut;
	COLORREF	m_crChatNull;
	COLORREF	m_crSearchNull;
	COLORREF	m_crSearchExists;
	COLORREF	m_crSearchExistsHit;
	COLORREF	m_crSearchExistsSelected;
	COLORREF	m_crSearchQueued;
	COLORREF	m_crSearchQueuedHit;
	COLORREF	m_crSearchQueuedSelected;
	COLORREF	m_crSearchGhostrated;
	COLORREF	m_crSearchHighrated;
	COLORREF	m_crSearchCollection;
	COLORREF	m_crSearchTorrent;
	COLORREF	m_crTransferSource;
	COLORREF	m_crTransferRanges;
	COLORREF	m_crTransferCompleted;
	COLORREF	m_crTransferVerifyPass;
	COLORREF	m_crTransferVerifyFail;
	COLORREF	m_crTransferCompletedSelected;
	COLORREF	m_crTransferVerifyPassSelected;
	COLORREF	m_crTransferVerifyFailSelected;
	COLORREF	m_crNetworkNull;
	COLORREF	m_crNetworkG1;
	COLORREF	m_crNetworkG2;
	COLORREF	m_crNetworkED2K;
	COLORREF	m_crNetworkUp;
	COLORREF	m_crNetworkDown;
	COLORREF	m_crSecurityAllow;
	COLORREF	m_crSecurityDeny;

	COLORREF	m_crDropdownBox;
	COLORREF	m_crDropdownText;
 	COLORREF	m_crResizebarEdge;
	COLORREF	m_crResizebarFace;
	COLORREF	m_crResizebarShadow;
	COLORREF	m_crResizebarHighlight;
	COLORREF	m_crFragmentShaded;
	COLORREF	m_crFragmentComplete;
	COLORREF	m_crFragmentSource1;
	COLORREF	m_crFragmentSource2;
	COLORREF	m_crFragmentSource3;
	COLORREF	m_crFragmentSource4;
	COLORREF	m_crFragmentSource5;
	COLORREF	m_crFragmentSource6;
	COLORREF	m_crFragmentPass;
	COLORREF	m_crFragmentFail;
	COLORREF	m_crFragmentRequest;
	COLORREF	m_crFragmentBorder;
	COLORREF	m_crFragmentBorderSelected;
	COLORREF	m_crFragmentBorderSimpleBar;
	COLORREF	m_crFragmentBorderSimpleBarSelected;

	COLORREF	m_crSysWindow;
	COLORREF	m_crSysBtnFace;
	COLORREF	m_crSysBorders;
	COLORREF	m_crSys3DShadow;
	COLORREF	m_crSys3DHighlight;
	COLORREF	m_crSysActiveCaption;

	void		Load();
	void		Clear();
	void		NameCommand(UINT nID, LPCTSTR pszName);
	UINT		NameToID(LPCTSTR pszName) const;
	int			ImageForID(UINT nID, int nImageListType = LVSIL_SMALL) const;
	void		AddIcon(UINT nID, HICON hIcon, int nImageListType = LVSIL_SMALL);
	void		CopyIcon(UINT nFromID, UINT nToID, int nImageListType = LVSIL_SMALL);
	HICON		ExtractIcon(UINT nID, BOOL bMirrored, int nImageListType = LVSIL_SMALL);
	int			ExtractIconID(UINT nID, BOOL bMirrored, int nImageListType = LVSIL_SMALL);
	// Set skinned icon to window i.e. pWnd->SetIcon( hIcon, bBigIcon )
	void		SetIcon(UINT nID, BOOL bMirrored, BOOL bBigIcon, CWnd* pWnd);
	// Set skinned icon to window i.e. pWnd->SetIcon( hIcon, bBigIcon )
	void		SetIcon(HICON hIcon, BOOL bMirrored, BOOL bBigIcon, CWnd* pWnd);
	//	BOOL	AddImagesFromToolbar(UINT nIDToolBar, COLORREF crBack = RGB(0,255,0));
	int			GetImageCount(int nImageListType = LVSIL_SMALL);
	BOOL		Add(CSkin* pSkin, CXMLElement* pBase, HBITMAP hbmImage, COLORREF crMask, int nImageListType = LVSIL_SMALL);
	CImageList*	SetImageListTo(CListCtrl& pWnd, int nImageListType = LVSIL_SMALL);
	BOOL		Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle = ILD_NORMAL, int nImageListType = LVSIL_SMALL) const;
	BOOL		DrawEx(CDC* pDC, int nImage, POINT pt, SIZE sz = CSize( 16, 16 ), COLORREF clrBk = CLR_NONE, COLORREF clrFg = CLR_DEFAULT, UINT nStyle = ILD_NORMAL, int nImageListType = LVSIL_SMALL) const;
	BOOL		Draw(CDC* pDC, UINT nID, int nSize, int nX, int nY, COLORREF crBack = CLR_NONE, BOOL bSelected = FALSE, BOOL bExclude = TRUE) const;
	CDC*		GetBuffer(CDC& dcScreen, CSize& szItem);
	BOOL		DrawWatermark(CDC* pDC, CRect* pRect, CBitmap* pMark, int nOffX = 0, int nOffY = 0);
	void		CreateFonts(LPCTSTR pszFace = NULL, int nSize = 0);
	void		CalculateColors(BOOL bCustom = FALSE);
	void		OnSysColorChange();
	static COLORREF	CalculateColor(COLORREF crFore, COLORREF crBack, int nAlpha);
	static COLORREF	GetDialogBkColor();
	static BOOL		IsNewWindows();
	static BOOL		EnableTheme(CWnd* pWnd, BOOL bEnable = TRUE);

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
