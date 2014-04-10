//
// CtrlUploads.h
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

#include "CtrlUploadTip.h"

class CUploadFile;
class CUploadQueue;
class CUploadDisplayData;
class CQueueDisplayData;


class CUploadsCtrl : public CWnd
{
	DECLARE_DYNAMIC(CUploadsCtrl)

public:
	CUploadsCtrl();

public:
	BOOL		Create(CWnd* pParentWnd, UINT nID);
	BOOL		Update();
protected:
	void		InsertColumn(int nColumn, LPCTSTR pszCaption, int nFormat, int nWidth);
	void		SaveColumnState();
	BOOL		LoadColumnState();
	void		SelectTo(int nIndex);
	void		DeselectAll(CUploadFile* pExcept = NULL);
	BOOL		HitTest(const CPoint& point, CUploadQueue** ppQueue, CUploadFile** ppFile, int* pnIndex, RECT* prcItem);
	BOOL		GetAt(int nSelect, CUploadQueue** ppQueue, CUploadFile** ppFile);
	void		PaintQueue(CDC& dc, const CRect& rcRow, const CQueueDisplayData* pQueueData, BOOL bFocus);
	void		PaintFile(CDC& dc, const CRect& rcRow, const CUploadDisplayData* pUploadData, BOOL bFocus);
	void		UpdateUploadsData(BOOL bForce = FALSE);
	int			GetExpandableColumnX() const;
	void		OnSkinChange();

	friend class CUploadsWnd;

protected:
	CHeaderCtrl			m_wndHeader;
	CUploadTipCtrl		m_wndTip;
	CUploadFile*		m_pDeselect;
	CImageList			m_gdiProtocols;
	int					m_nFocus;
	int 				m_nHover;			// Visible index

	CArray< CQueueDisplayData > m_pDisplayData;

// Queue/File Abstractation Layer
public:
	static POSITION			GetQueueIterator();
	static CUploadQueue*	GetNextQueue(POSITION& pos);
	static POSITION			GetFileIterator(CUploadQueue* pQueue);
	static CUploadFile*		GetNextFile(CUploadQueue* pQueue, POSITION& pos, int* pnPosition = NULL);

public:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
	afx_msg void OnChangeHeader(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();

protected:
	DECLARE_MESSAGE_MAP()
};


#include "UploadTransferHTTP.h"

class CUploadDisplayData
{
public:
	CUploadDisplayData();
	CUploadDisplayData(const CUploadFile* pUpload, CUploadTransfer* pTransfer);
	CUploadDisplayData& operator=(const CUploadDisplayData& pUpload);

public:
	CString			m_sName;				// pUpload->m_sName
	CString			m_sPath;				// pUpload->m_sPath
	QWORD			m_nSize;				// pUpload->m_nSize
	QWORD			m_nUploaded;			// pTransfer->m_nUploaded
	DWORD			m_nSpeed;				// pTransfer->GetMeasuredSpeed()
	DWORD			m_nProtocol;			// pTransfer->m_nProtocol
	BOOL			m_bSelected;			// pUpload->m_bSelected
	BOOL			m_bTorrent;				// pTransfer->m_nProtocol == PROTOCOL_BT
	BOOL			m_bTransferNull;		// pTransfer == NULL || pTransfer->m_nState == upsNull
	BOOL			m_bBaseFile;			// pUpload == pUploadTransfer->m_pBaseFile
	BOOL			m_bBackwards;			// pUpload->IsBackwards() && PROTOCOL_HTTP
	CString			m_sState;				// (Speed column LoadString)
	CString			m_sCountry;				// pTransfer->m_sCountry
	CString			m_sAddress;				// pTransfer->m_sAddress
	CString			m_sRemoteNick;			// pTransfer->m_sRemoteNick
	CString			m_sUserAgent;			// pTransfer->m_sUserAgent
	DWORD			m_nUserRating;			// pTransfer->m_nUserRating
	QWORD			m_nLength;				// pTransfer->m_nLength
	QWORD			m_nOffset;				// pTransfer->m_nOffset
	QWORD			m_nPosition;			// pTransfer->m_nPosition
	Fragments::List	m_oFragments;			// pFile->m_oFragments
};


class CQueueDisplayData
{
public:
	CQueueDisplayData();
	CQueueDisplayData(const CUploadQueue* pQueue);
	CQueueDisplayData& operator=(const CQueueDisplayData& pQueue);

public:
	CString			m_sName;				// pQueue->m_sName
	BOOL			m_bSelected;			// pQueue->m_bSelected
	BOOL			m_bExpanded;			// pQueue->m_bExpanded
	BOOL			m_bHistoryQueue;		// pQueue == UploadQueues.m_pHistoryQueue
	BOOL			m_bTorrentQueue;		// pQueue == UploadQueues.m_pTorrentQueue
	BOOL			m_bHTTPQueue;			// pQueue->m_nProtocols == ( 1 << PROTOCOL_HTTP )
	BOOL			m_bED2KQueue;			// pQueue->m_nProtocols == ( 1 << PROTOCOL_ED2K )
	DWORD			m_nMinTransfers;		// pQueue->GetTransferCount()	(Torrent: pQueue->m_nMinTransfers)
	DWORD			m_nMaxTransfers;		// pQueue->GetQueuedCount() 	(Torrent: pQueue->m_nMaxTransfers)
	DWORD			m_nSpeed;				// pQueue->GetMeasuredSpeed()
	DWORD			m_nCount;				// Uploads
	CArray< CUploadDisplayData > m_pUploadsData;
};


#define ULF_ACTIVE		0x01
#define ULF_QUEUED		0x02
#define ULF_HISTORY		0x04
#define ULF_TORRENT		0x08

#define ULF_ALL			(ULF_ACTIVE|ULF_QUEUED|ULF_HISTORY|ULF_TORRENT)
