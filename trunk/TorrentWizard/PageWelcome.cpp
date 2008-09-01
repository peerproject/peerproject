//
// PageWelcome.cpp
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
#include "PageWelcome.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWelcomePage, CWizardPage)

BEGIN_MESSAGE_MAP(CWelcomePage, CWizardPage)
	//{{AFX_MSG_MAP(CWelcomePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWelcomePage property page

CWelcomePage::CWelcomePage() : CWizardPage(CWelcomePage::IDD)
{
	//{{AFX_DATA_INIT(CWelcomePage)
	m_nType = 0;
	//}}AFX_DATA_INIT
}

CWelcomePage::~CWelcomePage()
{
}

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWelcomePage)
	DDX_Radio(pDX, IDC_TYPE_SINGLE, m_nType);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage message handlers

void CWelcomePage::OnReset()
{
	m_nType = 0;
	UpdateData( FALSE );
}

BOOL CWelcomePage::OnSetActive() 
{
	SetWizardButtons( PSWIZB_NEXT );
	return CWizardPage::OnSetActive();
}

LRESULT CWelcomePage::OnWizardNext() 
{
	UpdateData();
	
	if ( m_nType < 0 )
	{
		AfxMessageBox( IDS_WELCOME_NEED_TYPE, MB_ICONEXCLAMATION );
		return -1;
	}
	
	return m_nType ? IDD_PACKAGE_PAGE : IDD_SINGLE_PAGE;
}

