//
// PageComment.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008
// Portions Copyright Shareaza Development Team, 2007.
//
// PeerProject Torrent Wizard is free software; you can redistribute it
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// Torrent Wizard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#include "StdAfx.h"
#include "TorrentWizard.h"
#include "PageComment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCommentPage, CWizardPage)

BEGIN_MESSAGE_MAP(CCommentPage, CWizardPage)
	//{{AFX_MSG_MAP(CCommentPage)
	ON_WM_XBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCommentPage property page

CCommentPage::CCommentPage() : CWizardPage(CCommentPage::IDD)
{
	//{{AFX_DATA_INIT(CCommentPage)
	m_sComment = _T("");
	//}}AFX_DATA_INIT
}

CCommentPage::~CCommentPage()
{
}

void CCommentPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommentPage)
	DDX_Text(pDX, IDC_COMMENT, m_sComment);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCommentPage message handlers

void CCommentPage::OnReset()
{
	// Nothing here
}

BOOL CCommentPage::OnSetActive()
{
	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

LRESULT CCommentPage::OnWizardBack()
{
	return IDD_TRACKER_PAGE;
}

LRESULT CCommentPage::OnWizardNext()
{
	return IDD_OUTPUT_PAGE;
}

void CCommentPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}