//
// CtrlBrowseProfile.h
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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

#include "RichViewCtrl.h"
#include "ImageServices.h"
#include "ImageFile.h"

#define IDC_BROWSE_PROFILE	124

class CHostBrowser;
class CGProfile;
class CG2Packet;


class CBrowseProfileCtrl : public CWnd
{
// Construction
public:
	CBrowseProfileCtrl();
	virtual ~CBrowseProfileCtrl();

// Attributes
protected:
	CCriticalSection	m_pSection;
	CRichViewCtrl		m_wndDoc1;
	CRichViewCtrl		m_wndDoc2;
	CRichDocument*		m_pDocument1;
	CRichDocument*		m_pDocument2;
	CRichElement*		m_pdNick;
	CRichElement*		m_pdFullName;
	CRichElement*		m_pdFullLocation;
	CRichElement*		m_pdGenderMale;
	CRichElement*		m_pdGenderFemale;
	CRichElement*		m_pdAge;
	CRichElement*		m_pdBioText;
	CRichElement*		m_pdInterests;
	CRichElement*		m_pdBookmarks;
	CRichElement*		m_pdContactEmail;
	CRichElement*		m_pdContactMSN;
	CRichElement*		m_pdContactYahoo;
	CRichElement*		m_pdContactICQ;
	CRichElement*		m_pdContactAOL;
	CRichElement*		m_pdContactJabber;
	CRichElement*		m_pdContactTwitter;
	CRichElement*		m_pdContactFacebook;
	CRichElement*		m_pdContactPeerProjectOrg;
//	CRichElement*		m_pdCountryName;
//	CRichElement*		m_pdVendor;

	CImageFile			m_imgHead;

// Operations
public:
	void	Serialize(CArchive& ar, int nVersion);	// BROWSER_SER_VERSION
	void	OnSkinChange();
	void	OnHeadPacket(CG2Packet* pPacket);
	void	Update(CHostBrowser* pBrowser);
protected:
	void	UpdateDocument1(CGProfile* pProfile);
	void	UpdateDocument2(CHostBrowser* pBrowser);
	void	LoadDefaultHead();

// Overrides
public:
	//{{AFX_VIRTUAL(CBrowseProfileCtrl)
	virtual BOOL Create(CWnd* pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CBrowseProfileCtrl)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickView(NMHDR* pNotify, LRESULT *pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
