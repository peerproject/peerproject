//
// PageSettingsNetworks.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2011
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

#include "StdAfx.h"
#include "Settings.h"
#include "PeerProject.h"
#include "WndSettingsSheet.h"
#include "PageSettingsNetworks.h"
#include "PageSettingsGnutella.h"
#include "PageSettingsDonkey.h"
#include "Colors.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CNetworksSettingsPage, CSettingsPage)

BEGIN_MESSAGE_MAP(CNetworksSettingsPage, CSettingsPage)
	//{{AFX_MSG_MAP(CNetworksSettingsPage)
	ON_BN_CLICKED(IDC_G2_ENABLE, OnG2Enable)
	ON_BN_CLICKED(IDC_G1_ENABLE, OnG1Enable)
	ON_BN_CLICKED(IDC_ED2K_ENABLE, OnEd2kEnable)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CNetworksSettingsPage property page

CNetworksSettingsPage::CNetworksSettingsPage()
	: CSettingsPage( CNetworksSettingsPage::IDD )
	, m_bG2Enable ( FALSE )
	, m_bG1Enable ( FALSE )
	, m_bEDEnable ( FALSE )
{
	//{{AFX_DATA_INIT(CNetworksSettingsPage)
	//}}AFX_DATA_INIT
}

CNetworksSettingsPage::~CNetworksSettingsPage()
{
}

void CNetworksSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworksSettingsPage)
	DDX_Control(pDX, IDC_ED2K_SETUP, m_wndEDSetup);
	DDX_Control(pDX, IDC_G1_SETUP, m_wndG1Setup);
	DDX_Control(pDX, IDC_G2_SETUP, m_wndG2Setup);
	DDX_Check(pDX, IDC_G2_ENABLE, m_bG2Enable);
	DDX_Check(pDX, IDC_G1_ENABLE, m_bG1Enable);
	DDX_Check(pDX, IDC_ED2K_ENABLE, m_bEDEnable);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CNetworksSettingsPage message handlers

BOOL CNetworksSettingsPage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	m_bG2Enable = Settings.Gnutella2.Enabled;
	m_bG1Enable = Settings.Gnutella1.Enabled;
	m_bEDEnable = Settings.eDonkey.Enabled;

	UpdateData( FALSE );

	if ( Settings.Experimental.LAN_Mode )	// #ifdef LAN_MODE
	{
		GetDlgItem( IDC_G2_ENABLE )->EnableWindow( FALSE );
		GetDlgItem( IDC_G1_ENABLE )->EnableWindow( FALSE );
		GetDlgItem( IDC_ED2K_ENABLE )->EnableWindow( FALSE );
		m_wndG2Setup.EnableWindow( FALSE );
		m_wndG1Setup.EnableWindow( FALSE );
		m_wndEDSetup.EnableWindow( FALSE );
	}

	return TRUE;
}

HBRUSH CNetworksSettingsPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSettingsPage::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd == &m_wndG2Setup || pWnd == &m_wndG1Setup || pWnd == &m_wndEDSetup )
	{
		if ( pWnd->IsWindowEnabled() )
		{
			pDC->SetTextColor( Colors.m_crTextLink );
			pDC->SelectObject( &theApp.m_gdiFontLine );
		}
	}

	return hbr;
}

BOOL CNetworksSettingsPage::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CWnd* pLinks[] = { &m_wndG2Setup, &m_wndG1Setup, &m_wndEDSetup, NULL };
	CPoint point;
	CRect rc;

	GetCursorPos( &point );

	for ( int nLink = 0 ; pLinks[ nLink ] ; nLink++ )
	{
		pLinks[ nLink ]->GetWindowRect( &rc );

		if ( pLinks[ nLink ]->IsWindowEnabled() && rc.PtInRect( point ) )
		{
			SetCursor( theApp.LoadCursor( IDC_HAND ) );
			return TRUE;
		}
	}

	return CSettingsPage::OnSetCursor( pWnd, nHitTest, message );
}

void CNetworksSettingsPage::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rc;

	ClientToScreen( &point );

	m_wndG2Setup.GetWindowRect( &rc );
	if ( m_wndG2Setup.IsWindowEnabled() && rc.PtInRect( point ) )
		GetSheet()->SetActivePage( GetSheet()->GetPage( RUNTIME_CLASS(CGnutellaSettingsPage) ) );

	m_wndG1Setup.GetWindowRect( &rc );
	if ( m_wndG1Setup.IsWindowEnabled() && rc.PtInRect( point ) )
		GetSheet()->SetActivePage( GetSheet()->GetPage( RUNTIME_CLASS(CGnutellaSettingsPage) ) );

	m_wndEDSetup.GetWindowRect( &rc );
	if ( m_wndEDSetup.IsWindowEnabled() && rc.PtInRect( point ) )
		GetSheet()->SetActivePage( GetSheet()->GetPage( RUNTIME_CLASS(CDonkeySettingsPage) ) );

	CSettingsPage::OnLButtonUp( nFlags, point );
}

BOOL CNetworksSettingsPage::OnSetActive()
{
	CGnutellaSettingsPage* ppGnutella =
		(CGnutellaSettingsPage*)GetPage( RUNTIME_CLASS(CGnutellaSettingsPage) );

	if ( ppGnutella->GetSafeHwnd() != NULL )
	{
		ppGnutella->UpdateData();
		m_bG2Enable = ppGnutella->m_bG2Today;
		m_bG1Enable = ppGnutella->m_bG1Today;
	}

	CDonkeySettingsPage* ppDonkey =
		(CDonkeySettingsPage*)GetPage( RUNTIME_CLASS(CDonkeySettingsPage) );

	if ( ppDonkey->GetSafeHwnd() != NULL )
	{
		ppDonkey->UpdateData();
		m_bEDEnable = ppDonkey->m_bEnabled;
	}

	UpdateData( FALSE );
	// m_wndG2Setup.EnableWindow( m_bG2Enable );
	// m_wndG1Setup.EnableWindow( m_bG1Enable );
	// m_wndEDSetup.EnableWindow( m_bEDEnable );

	return CSettingsPage::OnSetActive();
}

void CNetworksSettingsPage::OnG2Enable()
{
	UpdateData();

	if ( ! m_bG2Enable )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_DISABLE_G2 );

		if ( AfxMessageBox( strMessage, MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2 ) != IDYES )
		{
			m_bG2Enable = TRUE;
			UpdateData( FALSE );
		}
	}

	CGnutellaSettingsPage* ppGnutella =
		(CGnutellaSettingsPage*)GetPage( RUNTIME_CLASS(CGnutellaSettingsPage) );

	if ( ppGnutella->GetSafeHwnd() != NULL )
	{
		ppGnutella->UpdateData( TRUE );
		ppGnutella->m_bG2Today = m_bG2Enable;
		ppGnutella->UpdateData( FALSE );
	}

	// m_wndG2Setup.EnableWindow( m_bG2Enable );
}

void CNetworksSettingsPage::OnG1Enable()
{
	UpdateData();

	if ( m_bG1Enable && ( Settings.GetOutgoingBandwidth() < 2 ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_BANDWIDTH_LOW );
		AfxMessageBox( strMessage, MB_OK );
		m_bG1Enable = FALSE;
		UpdateData( FALSE );
	}

	CGnutellaSettingsPage* ppGnutella =
		(CGnutellaSettingsPage*)GetPage( RUNTIME_CLASS(CGnutellaSettingsPage) );

	if ( ppGnutella->GetSafeHwnd() != NULL )
	{
		ppGnutella->UpdateData( TRUE );
		ppGnutella->m_bG1Today = m_bG1Enable;
		ppGnutella->UpdateData( FALSE );
	}

	// m_wndG1Setup.EnableWindow( m_bG1Enable );
}

void CNetworksSettingsPage::OnEd2kEnable()
{
	UpdateData();

	if ( m_bEDEnable && ( Settings.GetOutgoingBandwidth() < 2 ) )
	{
		CString strMessage;
		LoadString( strMessage, IDS_NETWORK_BANDWIDTH_LOW );
		AfxMessageBox( strMessage, MB_OK );
		m_bEDEnable = FALSE;
		UpdateData( FALSE );
	}

	CDonkeySettingsPage* ppDonkey =
		(CDonkeySettingsPage*)GetPage( RUNTIME_CLASS(CDonkeySettingsPage) );

	if ( ppDonkey->GetSafeHwnd() != NULL )
	{
		ppDonkey->UpdateData( TRUE );
		ppDonkey->m_bEnabled = m_bEDEnable;
		ppDonkey->UpdateData( FALSE );
	}

	// m_wndEDSetup.EnableWindow( m_bEDEnable );
}

void CNetworksSettingsPage::OnOK()
{
	UpdateData( TRUE );

	Settings.Gnutella2.Enabled	= m_bG2Enable != FALSE;
	Settings.Gnutella1.Enabled	= m_bG1Enable != FALSE;
	Settings.eDonkey.Enabled	= m_bEDEnable != FALSE;

	CSettingsPage::OnOK();
}
