//
// DlgDiscoveryService.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2011
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
#include "DlgDiscoveryService.h"
#include "DiscoveryServices.h"
#include "Network.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

BEGIN_MESSAGE_MAP(CDiscoveryServiceDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CDiscoveryServiceDlg)
	ON_EN_CHANGE(IDC_ADDRESS, OnChangeAddress)
	ON_CBN_SELCHANGE(IDC_SERVICE_TYPE, OnSelChangeServiceType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDiscoveryServiceDlg dialog

CDiscoveryServiceDlg::CDiscoveryServiceDlg(CWnd* pParent, CDiscoveryService* pService) : CSkinDialog(CDiscoveryServiceDlg::IDD, pParent)
	, m_pService	( pService )
	, m_sAddress	( _T("") )
	, m_nType 		( -1 )
	, m_bNew		( FALSE )
{
	//{{AFX_DATA_INIT(CDiscoveryServiceDlg)
	//}}AFX_DATA_INIT
}

CDiscoveryServiceDlg::~CDiscoveryServiceDlg()
{
	if ( m_pService && m_bNew ) delete m_pService;
}

void CDiscoveryServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiscoveryServiceDlg)
	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Text(pDX, IDC_ADDRESS, m_sAddress);
	DDX_CBIndex(pDX, IDC_SERVICE_TYPE, m_nType);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CDiscoveryServiceDlg message handlers

BOOL CDiscoveryServiceDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( _T("CDiscoveryServiceDlg"), IDR_DISCOVERYFRAME );

	CSingleLock pLock( &Network.m_pSection, TRUE );

	m_bNew = ! DiscoveryServices.Check( m_pService );
	if ( m_bNew ) m_pService = new CDiscoveryService();

	m_sAddress	= m_pService->m_sAddress;
	m_nType		= m_pService->m_nType - 1;

	// Reassigning the combo-box placeholder:
	if ( m_nType == 1 )
	{
		if ( m_pService->m_bGnutella1 && m_pService->m_bGnutella2 )
			m_nType = 3;
		else if ( m_pService->m_bGnutella2 )
			m_nType = 2;
	}
	else if ( m_nType > 1 )
	{
		m_nType += 2;
	}

	if ( m_bNew )
		m_nType = 1;

	pLock.Unlock();

	UpdateData( FALSE );

	OnChangeAddress();

	return TRUE;
}

void CDiscoveryServiceDlg::OnChangeAddress()
{
	UpdateData();

	m_wndOK.EnableWindow( m_nType ?
		_tcsncmp( m_sAddress, _T("http://"), 7 ) == 0 :
		_tcschr( m_sAddress, '.' ) != NULL );
}

void CDiscoveryServiceDlg::OnSelChangeServiceType()
{
	OnChangeAddress();
}

void CDiscoveryServiceDlg::OnOK()
{
	UpdateData();

	CSingleLock pLock( &Network.m_pSection, TRUE );

	if ( ! m_bNew && ! DiscoveryServices.Check( m_pService ) )
		m_pService = new CDiscoveryService();

	m_pService->m_sAddress = m_sAddress;

	switch( m_nType )
	{
	case 0:
		m_pService->m_nType = CDiscoveryService::dsGnutella;
		break;
	case 1:
		m_pService->m_nType = CDiscoveryService::dsWebCache;
		m_pService->m_bGnutella1 = TRUE;
		m_pService->m_bGnutella2 = FALSE;
		m_pService->m_nProtocolID = PROTOCOL_G1;
		break;
	case 2:
		m_pService->m_nType = CDiscoveryService::dsWebCache;
		m_pService->m_bGnutella1 = FALSE;
		m_pService->m_bGnutella2 = TRUE;
		m_pService->m_nProtocolID = PROTOCOL_G2;
		break;
	case 3:
		m_pService->m_nType = CDiscoveryService::dsWebCache;
		m_pService->m_bGnutella1 = TRUE;
		m_pService->m_bGnutella2 = TRUE;
		m_pService->m_nProtocolID = PROTOCOL_ANY;
		break;
	case 4:
		m_pService->m_nType = CDiscoveryService::dsServerList;
		m_pService->m_nProtocolID =
			( m_sAddress.Find( _T("hublist"), 6 ) > 6 || m_sAddress.Find( _T(".xml.bz2"), 8 ) > 8 ) ?
			PROTOCOL_DC : PROTOCOL_ED2K;
		break;
	default:
		m_pService->m_nType = CDiscoveryService::dsBlocked;
		m_pService->m_nProtocolID = PROTOCOL_NULL;
	}

	if ( m_pService->m_nType == CDiscoveryService::dsGnutella )
	{
		if ( _tcsnicmp( m_sAddress, _T("gnutella2:host:"), 15 ) == 0 ||
			 _tcsnicmp( m_sAddress, _T("g2:host:"), 8 ) == 0 )
		{
			m_pService->m_bGnutella2	= TRUE;
			m_pService->m_bGnutella1	= FALSE;
			m_pService->m_nProtocolID	= PROTOCOL_G2;
			m_pService->m_nSubType		= CDiscoveryService::dsGnutella2TCP;
		}
		else if ( _tcsnicmp( m_sAddress, _T("gnutella1:host:"), 15 ) == 0 ||
			 _tcsnicmp( m_sAddress, _T("gnutella:host:"), 14 ) == 0 )
		{
			m_pService->m_bGnutella2	= FALSE;
			m_pService->m_bGnutella1	= TRUE;
			m_pService->m_nProtocolID	= PROTOCOL_G1;
			m_pService->m_nSubType		= CDiscoveryService::dsGnutellaTCP;
		}
		else if ( _tcsnicmp( m_sAddress, _T("uhc:"), 4 ) == 0 )
		{
			m_pService->m_bGnutella2	= FALSE;
			m_pService->m_bGnutella1	= TRUE;
			m_pService->m_nProtocolID	= PROTOCOL_G1;
			m_pService->m_nSubType		= CDiscoveryService::dsGnutellaUDPHC;
		}
		else if ( _tcsnicmp( m_sAddress, _T("ukhl:"), 5 ) == 0 )
		{
			m_pService->m_bGnutella2	= TRUE;
			m_pService->m_bGnutella1	= FALSE;
			m_pService->m_nProtocolID	= PROTOCOL_G2;
			m_pService->m_nSubType		= CDiscoveryService::dsGnutella2UDPKHL;
		}
	}

	DiscoveryServices.Add( m_pService );
	m_pService = NULL;

	pLock.Unlock();

	CSkinDialog::OnOK();
}
