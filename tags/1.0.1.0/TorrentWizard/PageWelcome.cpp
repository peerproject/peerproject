//
// PageWelcome.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) © 2008,2012-2014
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
	//ON_BN_CLICKED(IDC_EXPERT_MODE, OnExpertMode)
	ON_WM_XBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWelcomePage property page

CWelcomePage::CWelcomePage() : CWizardPage(CWelcomePage::IDD)
{
	m_nType = theApp.GetProfileInt( L"", L"Mode", 0 );
}

//CWelcomePage::~CWelcomePage()
//{
//}

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_TYPE_SINGLE, m_nType);
}

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage message handlers

void CWelcomePage::OnReset()
{
	m_nType = theApp.GetProfileInt( L"", L"Mode", 0 );
	UpdateData( FALSE );
}

BOOL CWelcomePage::OnSetActive()
{
	SetWizardButtons( PSWIZB_NEXT );

//	GetSheet()->GetDlgItem( 2 )->EnableWindow( TRUE );

	if ( ! theApp.m_sCommandLineSourceFile.IsEmpty() )
	{
		m_nType = PathIsDirectory( theApp.m_sCommandLineSourceFile ) ? 1 : 0;
		Next();
	}

	UpdateData( FALSE );

	if ( m_nType == 2 && theApp.GetProfileInt( L"", L"Expert", FALSE ) == TRUE )
		Next();

	return CWizardPage::OnSetActive();
}

void CWelcomePage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}

LRESULT CWelcomePage::OnWizardNext()
{
	UpdateData();

	theApp.WriteProfileInt( L"", L"Mode", m_nType );
	if ( m_nType == 2 )
		theApp.WriteProfileInt( L"", L"Expert", TRUE );

	if ( m_nType == 0 )
		return IDD_SINGLE_PAGE;
	if ( m_nType == 1 )
		return IDD_PACKAGE_PAGE;
	if ( m_nType == 2 )
		return IDD_EXPERT_PAGE;

	return -1;
}
