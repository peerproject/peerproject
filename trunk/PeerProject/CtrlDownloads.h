//
// CtrlDownloads.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2014
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software. You may redistribute and/or modify it
// under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// version 3 or later at your option. (AGPLv3)
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

#include "CtrlDownloadTip.h"
#include "FileFragments.hpp"

class CDownload;
class CDownloadSource;
class CDownloadDisplayData;
class CSourceDisplayData;


class CDownloadsCtrl : public CWnd
{
	DECLARE_DYNAMIC(CDownloadsCtrl)

public:
	CDownloadsCtrl();

public:
	BOOL		Create(CWnd* pParentWnd, UINT nID);
	BOOL		Update();
	BOOL		Update(int nGroupCookie);
	int 		GetExpandableColumnX() const;
	BOOL		DropObjects(CList< CDownload* >* pSel, const CPoint& ptScreen);
	void		OnMouseMoveDrag(const CPoint& ptScreen);	// Was DropShowTarget(CList< CDownload* >* pSel, const CPoint& ptScreen)
	void		OnSkinChange();
protected:
	void		InsertColumn(int nColumn, LPCTSTR pszCaption, int nFormat, int nWidth);
	void		SaveColumnState();
	BOOL		LoadColumnState();
	void		SelectTo(int nIndex);
	void		BubbleSortDownloads(int nColumn);
	void		SelectAll(CDownload* pExcept1 = NULL, CDownloadSource* pExcept2 = NULL);
	void		DeselectAll(CDownload* pExcept1 = NULL, CDownloadSource* pExcept2 = NULL);
	BOOL		GetSelectedList(CList< CDownload* >& pList, BOOL bClearing = FALSE);
	int 		GetSelectedCount();
	void		MoveSelected(int nDelta);
	void		MoveToTop();
	void		MoveToEnd();
	BOOL		HitTest(const CPoint& point, CDownload** ppDownload, CDownloadSource** ppSource, int* pnIndex, RECT* prcItem);
	BOOL		GetAt(int nSelect, CDownload** ppDownload, CDownloadSource** ppSource);
	BOOL		GetRect(CDownload* pSelect, RECT* prcItem);
	void		PaintDownload(CDC& dc, const CRect& rcRow, const CDownloadDisplayData* pDownloadData, BOOL bFocus = FALSE);
	void		PaintSource(CDC& dc, const CRect& rcRow, const CSourceDisplayData* pSourceData, BOOL bFocus = FALSE);
	void		UpdateDownloadsData(BOOL bForce = FALSE);
	void		OnBeginDrag(CPoint ptAction);
	CImageList*	CreateDragImage(CList< CDownload* >* pSel, const CPoint& ptMouse);	// ToDo: Remove List
public:
	static bool	IsFiltered(const CDownload* pDownload);
	static BOOL	IsExpandable(const CDownload* pDownload);

	friend class CDownloadsWnd;

protected:
	CHeaderCtrl			m_wndHeader;
	CDownloadTipCtrl	m_wndTip;
	CImageList			m_pProtocols;
	int 				m_nGroupCookie;
	int 				m_nFocus;
	int 				m_nHover;			// Visible index
//	CDownload*			m_pDragDrop;		// Use m_nHover
	BOOL				m_bCreateDragImage;
	BOOL				m_bDragStart;
	BOOL				m_bDragActive;
	CPoint				m_ptDrag;
	CDownload*			m_pDeselect1;
	CDownloadSource*	m_pDeselect2;
	BOOL*				m_pbSortAscending;
	BOOL				m_bShowSearching;
//	DWORD				m_tSwitchTimer;		// Using static

	CArray< CDownloadDisplayData > m_pDownloadsData;

public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChangeHeader(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSortPanelItems(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnterKey();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();

protected:
	DECLARE_MESSAGE_MAP()
};


class CSourceDisplayData
{
public:
	CSourceDisplayData();
	CSourceDisplayData(const CDownloadSource* pSource);
	CSourceDisplayData& operator=(const CSourceDisplayData& pSource);

public:
	BOOL			m_bSelected;			// pSource->m_bSelected
	DWORD			m_nProtocol;			// pSource->m_nProtocol
	QWORD			m_nSize;				// pDownload->m_nSize
	DWORD			m_nState;				// pSource->GetState()
	CString			m_sState;				// pSource->GetState( FALSE )
	DWORD			m_tAttempt;				// pSource->m_tAttempt
	BOOL			m_bTrying;				// pSource->m_tAttempt && pDownload->IsTrying()
	BOOL			m_bIdle;				// pSource->IsIdle()
	BOOL			m_bPushOnly;			// pSource->m_bPushOnly		(ed2k)
	QWORD			m_nDownloaded;			// pSource->GetDownloaded()
	DWORD			m_nSpeed;				// pSource->GetMeasuredSpeed()
	CString			m_sServer;				// pSource->m_sServer
	DWORD			m_nAddress;				// pSource->m_pAddress.S_un.S_addr
	CString			m_sAddress;				// CString( inet_ntoa( pSource->m_pAddress )	Or ed2k/dc: CString( inet_ntoa( pSource->m_pServerAddress )
	CString			m_sAddressGet;			// pSource->GetAddress()
	WORD			m_nServerPort;			// pSource->m_nServerPort
	WORD			m_nPort;				// pSource->m_nPort
	WORD			m_nPortGet;				// pSource->GetPort()
	CString			m_sNick;				// pSource->m_sNick
	CString			m_sCountry;				// pSource->m_sCountry
	int				m_nColor;				// m_pDownload->GetSourceColor() index
	BOOL			m_bReadContent;			// pSource->m_bReadContent
	BOOL			m_bHasFragments;		// IsOnline() && HasUsefulRanges() || ! m_oPastFragments.empty()
	BOOL			m_bTransferBackwards;	// m_pTransfer->m_bRecvBackwards
	DWORD			m_nTransferLength;		// m_pTransfer->m_nLength
	DWORD			m_nTransferOffset;		// m_pTransfer->m_nOffset
	DWORD			m_nTransferPosition;	// m_pTransfer->m_nPosition
	Fragments::List	m_oAvailable;			// pSource->m_oAvailable
	Fragments::List	m_oPastFragments;		// pSource->m_oPastFragments
};


class CDownloadDisplayData
{
public:
	CDownloadDisplayData();
	CDownloadDisplayData(const CDownload* pDownload);
	CDownloadDisplayData& operator=(const CDownloadDisplayData& pDownload);

struct VERIFYRANGE
{
	QWORD			nOffset;
	QWORD			nLength;
	BOOL			bSuccess;
};

public:
	QWORD			m_nSize;				// pDownload->m_nSize
	CString			m_sName;				// pDownload->m_sName
	CString			m_sDisplayName;			// pDownload->GetDisplayName()
	BOOL			m_bSelected;			// pDownload->m_bSelected
	BOOL			m_bExpanded;			// pDownload->m_bExpanded
	BOOL			m_bExpandable;			// IsExpandable(pDownload)
	BOOL			m_bTrying;				// pDownload->IsTrying()	(m_tBegan>0)
	BOOL			m_bClearing;			// pDownload->m_bClearing
	BOOL			m_bCompleted;			// pDownload->IsCompleted()
	BOOL			m_bFailedVerify;		// pDownload->m_bVerify == TRI_FALSE
	DWORD			m_nVolumeComplete;		// pDownload->GetVolumeComplete()		Or Seeding: pDownload->m_nTorrentUploaded
	float			m_fProgress;			// pDownload->GetProgress()
	float			m_fRatio;				// pDownload->GetRatio()	Seeding
	UINT			m_nRating;				// pDownload->GetReviewAverage()
	DWORD			m_nAverageSpeed;		// pDownload->GetAverageSpeed()	DWORD
	CString			m_sDownloadSources;		// pDownload->GetDownloadSources()
	BOOL			m_bMultiFileTorrent;	// pDownload->IsMultiFileTorrent()
	BOOL			m_bSeeding;				// pDownload->IsSeeding()
	BOOL			m_bSearching;			// pDownload->IsSearching()
	CString			m_sDownloadStatus;		// pDownload->GetDownloadStatus()
	DWORD			m_nSourceCount;			// pDownload->GetSourceCount()
	Fragments::List	m_oEmptyFragments;
	CArray< VERIFYRANGE > m_pVerifyRanges;	// pDownload->GetNextVerifyRange()
	CArray< CSourceDisplayData > m_pSourcesData;
};


#define DLF_ACTIVE		0x01
#define DLF_QUEUED		0x02
#define DLF_SOURCES		0x04
#define DLF_PAUSED		0x08
#define DLF_SEED		0x10

#define DLF_ALL			(DLF_ACTIVE|DLF_QUEUED|DLF_SOURCES|DLF_PAUSED|DLF_SEED)
