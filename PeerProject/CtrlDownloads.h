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
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar = NULL);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChangeHeader(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSortPanelItems(NMHDR* pNotifyStruct, LRESULT* pResult);
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


struct VERIFYRANGE
{
	QWORD			nOffset;
	QWORD			nLength;
	BOOL			bSuccess;
};

class CSourceDisplayData
{
public:
	CSourceDisplayData();
	CSourceDisplayData(const CDownloadSource* pSource);
	CSourceDisplayData& operator=(const CSourceDisplayData& pSource);

public:
	BOOL			bSelected;			// pSource->m_bSelected
	DWORD			nProtocol;			// pSource->m_nProtocol
	QWORD			nSize;				// pDownload->m_nSize
	DWORD			nState;				// pSource->GetState()
	CString			sState;				// pSource->GetState( FALSE )
	DWORD			tAttempt;			// pSource->m_tAttempt
	BOOL			bTrying;			// pSource->m_tAttempt && pDownload->IsTrying()
	BOOL			bIdle;				// pSource->IsIdle()
	BOOL			bPushOnly;			// pSource->m_bPushOnly		(ed2k)
	QWORD			nDownloaded;		// pSource->GetDownloaded()
	DWORD			nSpeed;				// pSource->GetMeasuredSpeed()
	CString			sServer;			// pSource->m_sServer
	DWORD			nAddress;			// pSource->m_pAddress.S_un.S_addr
	CString			sAddress;			// CString( inet_ntoa( pSource->m_pAddress )	Or ed2k/dc: CString( inet_ntoa( pSource->m_pServerAddress )
	CString			sAddressGet;		// pSource->GetAddress()
	WORD			nServerPort;		// pSource->m_nServerPort
	WORD			nPort;				// pSource->m_nPort
	WORD			nPortGet;			// pSource->GetPort()
	CString			sNick;				// pSource->m_sNick
	CString			sCountry;			// pSource->m_sCountry
	int				nColor;				// m_pDownload->GetSourceColor() index
	BOOL			bReadContent;		// pSource->m_bReadContent
	BOOL			bHasFragments;		// IsOnline() && HasUsefulRanges() || ! m_oPastFragments.empty()
	BOOL			bTransferBackwards;	// m_pTransfer->m_bRecvBackwards
	DWORD			nTransferLength;	// m_pTransfer->m_nLength
	DWORD			nTransferOffset;	// m_pTransfer->m_nOffset
	DWORD			nTransferPosition;	// m_pTransfer->m_nPosition
	Fragments::List	oAvailable;			// pSource->m_oAvailable
	Fragments::List	oPastFragments;		// pSource->m_oPastFragments
};


class CDownloadDisplayData
{
public:
	CDownloadDisplayData();
	CDownloadDisplayData(const CDownload* pDownload);
	CDownloadDisplayData& operator=(const CDownloadDisplayData& pDownload);

public:
	QWORD			nSize;				// pDownload->m_nSize
	CString			sName;				// pDownload->m_sName
	CString			sDisplayName;		// pDownload->GetDisplayName()
	BOOL			bSelected;			// pDownload->m_bSelected
	BOOL			bExpanded;			// pDownload->m_bExpanded
	BOOL			bExpandable;		// IsExpandable(pDownload)
	BOOL			bTrying;			// pDownload->IsTrying()	(m_tBegan>0)
	BOOL			bClearing;			// pDownload->m_bClearing
	BOOL			bCompleted;			// pDownload->IsCompleted()
	BOOL			bFailedVerify;		// pDownload->m_bVerify == TRI_FALSE
	DWORD			nVolumeComplete;	// pDownload->GetVolumeComplete()		Or Seeding: pDownload->m_nTorrentUploaded
	float			fProgress;			// pDownload->GetProgress()
	float			fRatio;				// pDownload->GetRatio()	Seeding
	UINT			nRating;			// pDownload->GetReviewAverage()		(pDownload->GetReviewCount())
	DWORD			nAverageSpeed;		// pDownload->GetAverageSpeed()	DWORD
	CString			sDownloadSources;	// pDownload->GetDownloadSources()
	BOOL			bMultiFileTorrent;	// pDownload->IsMultiFileTorrent()
	BOOL			bSeeding;			// pDownload->IsSeeding()
	BOOL			bSearching;			// pDownload->IsSearching()
	CString			sDownloadStatus;	// pDownload->GetDownloadStatus()
	DWORD			nSourceCount;		// pDownload->GetSourceCount()
	Fragments::List	oEmptyFragments;
	CArray< VERIFYRANGE > pVerifyRanges; // pDownload->GetNextVerifyRange()
	CArray< CSourceDisplayData > pSourcesData;
};


#define DLF_ACTIVE		0x01
#define DLF_QUEUED		0x02
#define DLF_SOURCES		0x04
#define DLF_PAUSED		0x08
#define DLF_SEED		0x10

#define DLF_ALL			(DLF_ACTIVE|DLF_QUEUED|DLF_SOURCES|DLF_PAUSED|DLF_SEED)
