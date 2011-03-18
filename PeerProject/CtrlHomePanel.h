//
// CtrlHomePanel.h
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

#include "CtrlRichTaskBox.h"
#include "CtrlDownloadTip.h"
#include "CtrlLibraryTip.h"

class CDownload;
class CLibraryRecent;


class CHomeConnectionBox : public CRichTaskBox
{
// Construction
public:
	CHomeConnectionBox();
	virtual ~CHomeConnectionBox();

	DECLARE_DYNAMIC(CHomeConnectionBox)

protected:
	CRichElement*	m_pdConnectedHours;
	CRichElement*	m_pdConnectedMinutes;
	CRichElement*	m_pdCount[PROTOCOL_LAST][3];
	CString			m_sCount[PROTOCOL_LAST][3];

public:
	void	OnSkinChange();
	void	Update();

protected:
	DECLARE_MESSAGE_MAP()
};


class CHomeLibraryBox : public CRichTaskBox
{
// Construction
public:
	CHomeLibraryBox();
	virtual ~CHomeLibraryBox();

	DECLARE_DYNAMIC(CHomeLibraryBox)

protected:
	class Item
	{
	public:
		inline Item() throw() :
			m_pRecent( NULL ),
			m_nIndex( 0 ),
			m_nIcon16( 0 ) {}

		CLibraryRecent*	m_pRecent;
		DWORD			m_nIndex;
		CString			m_sText;
		int				m_nIcon16;
	};

	CRichElement*	m_pdLibraryFiles;
	CRichElement*	m_pdLibraryVolume;
	CRichElement*	m_pdLibraryHashRemaining;
	CArray< Item* >	m_pList;
	CFont			m_pFont;
	Item*			m_pHover;
	HCURSOR			m_hHand;
	CLibraryTipCtrl	m_wndTip;

public:
	void	OnSkinChange();
	void	Update();
	Item*	HitTest(const CPoint& point) const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};


class CHomeDownloadsBox : public CRichTaskBox
{
// Construction
public:
	CHomeDownloadsBox();
	virtual ~CHomeDownloadsBox();

	DECLARE_DYNAMIC(CHomeDownloadsBox)

protected:
	class Item
	{
	public:
		inline Item () throw() :
			m_pDownload( NULL ),
			m_nIcon16( 0 ),
			m_nSize( 0 ),
			m_nComplete( 0 ),
			m_bPaused( FALSE ) {}

		CDownload*	m_pDownload;
		CString		m_sText;
		int			m_nIcon16;
		QWORD		m_nSize;
		QWORD		m_nComplete;
		BOOL		m_bPaused;
	};

	CRichElement*		m_pdDownloadsNone;
	CRichElement*		m_pdDownloadsOne;
	CRichElement*		m_pdDownloadsMany;
	CRichElement*		m_pdDownloadedNone;
	CRichElement*		m_pdDownloadedOne;
	CRichElement*		m_pdDownloadedMany;
	CRichElement*		m_pdDownloadedVolume;
	CString				m_sDownloadedMany;
	CString				m_sDownloadsMany;
	CArray< Item* >		m_pList;
	CFont				m_pFont;
	Item*				m_pHover;
	HCURSOR				m_hHand;
	CDownloadTipCtrl	m_wndTip;

public:
	void	OnSkinChange();
	void	Update();
	Item*	HitTest(const CPoint& point) const;
	BOOL	ExecuteDownload(CDownload* pDownload);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};


class CHomeUploadsBox : public CRichTaskBox
{
// Construction
public:
	CHomeUploadsBox();
	virtual ~CHomeUploadsBox();

	DECLARE_DYNAMIC(CHomeUploadsBox)

protected:
	CRichElement*	m_pdUploadsNone;
	CRichElement*	m_pdUploadsOne;
	CRichElement*	m_pdUploadsMany;
	CRichElement*	m_pdUploadedNone;
	CRichElement*	m_pdUploadedOne;
	CRichElement*	m_pdUploadedMany;
	CRichElement*	m_pdTorrentsOne;
	CRichElement*	m_pdTorrentsMany;
	CString			m_sUploadsMany;
	CString			m_sUploadedOne;
	CString			m_sUploadedMany;
	CString			m_sTorrentsMany;

public:
	void		OnSkinChange();
	void		Update();

protected:
	DECLARE_MESSAGE_MAP()
};


class CHomePanel : public CTaskPanel
{
// Construction
public:
	CHomePanel();

	DECLARE_DYNAMIC(CHomePanel)

public:
	CHomeConnectionBox	m_boxConnection;
	CHomeLibraryBox		m_boxLibrary;
	CHomeDownloadsBox	m_boxDownloads;
	CHomeUploadsBox		m_boxUploads;

	void	OnSkinChange();
	void	Update();

	virtual BOOL Create(CWnd* pParentWnd);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

#define IDC_HOME_PANEL	111
