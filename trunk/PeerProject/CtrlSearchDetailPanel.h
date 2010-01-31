//
// CtrlSearchDetailPanel.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
// Portions Copyright Shareaza Development Team, 2002-2008.
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

#include "CtrlPanel.h"
#include "MetaPanel.h"
#include "RichDocument.h"
#include "RichViewCtrl.h"
#include "HttpRequest.h"

class CMatchFile;
class CImageFile;
class CSearchDetailPanel;


class Review
{
public:
	Review(const Hashes::Guid& oGUID, IN_ADDR* pAddress, LPCTSTR pszNick, int nRating, LPCTSTR pszComments);
	virtual ~Review();
	void			Layout(CSearchDetailPanel* pParent, CRect* pRect);
	void			Reposition(int nScroll);
	void			Paint(CDC* pDC, int nScroll);

protected:
	Hashes::Guid	m_oGUID;
	CString			m_sNick;
	int				m_nRating;
	CRichDocument	m_pComments;
	CRichViewCtrl	m_wndComments;
	CRect			m_rc;
};


class CSearchDetailPanel :
	public CPanelCtrl,
	public CThreadImpl
{
	DECLARE_DYNAMIC(CSearchDetailPanel)

public:
	CSearchDetailPanel();
	virtual ~CSearchDetailPanel();

	virtual void Update();

	void		SetFile(CMatchFile* pFile);

protected:
	static void	DrawText(CDC* pDC, int nX, int nY, LPCTSTR pszText, RECT* pRect = NULL, int nMaxWidth = -1);
	void		ClearReviews();
	BOOL		RequestPreview();
	void		CancelPreview();
	void		OnRun();
	BOOL		ExecuteRequest(CString strURL, BYTE** ppBuffer, DWORD* pnBuffer);
    void		OnPreviewLoaded(const Hashes::Sha1Hash& oSHA1, CImageFile* pImage);
    BOOL		CachePreviewImage(const Hashes::Sha1Hash& oSHA1, LPBYTE pBuffer, DWORD nBuffer);

	friend class Review;

	CMatchList*			m_pMatches;
	BOOL				m_bValid;
	CMatchFile*			m_pFile;
	Hashes::Sha1Hash    m_oSHA1;
	CString				m_sName;
	CString				m_sStatus;
	CRect				m_rcStatus;
	CString				m_sSize;
	int					m_nIcon48;
	int					m_nIcon32;
	int					m_nRating;
	CSchemaPtr			m_pSchema;
	CMetaPanel			m_pMetadata;
	CList< Review* >	m_pReviews;
	CCriticalSection	m_pSection;
	BOOL				m_bCanPreview;
	BOOL				m_bRunPreview;
	BOOL				m_bIsPreviewing;
	CHttpRequest		m_pRequest;
	CList< CString >	m_pPreviewURLs;
	CBitmap				m_bmThumb;			// Thumbnail
	CRect				m_rcThumb;			// Thumbnail rect for mouse click detection

	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClickReview(NMHDR* pNotify, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

#define IDC_REVIEW_VIEW		99
