//
// DlgURLCopy.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "PeerProjectFile.h"
#include "DlgURLCopy.h"
#include "Transfer.h"
#include "Network.h"
#include "Colors.h"

#include "Download.h"	// &tr=
#include "Downloads.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CURLCopyDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CURLCopyDlg, CSkinDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_INCLUDE_SELF, &CURLCopyDlg::OnIncludeSelf)
	ON_STN_CLICKED(IDC_URL_HOST, &CURLCopyDlg::OnStnClickedUrlHost)
	ON_STN_CLICKED(IDC_URL_MAGNET, &CURLCopyDlg::OnStnClickedUrlMagnet)
	ON_STN_CLICKED(IDC_URL_ED2K, &CURLCopyDlg::OnStnClickedUrlEd2k)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CURLCopyDlg dialog

CURLCopyDlg::CURLCopyDlg(CWnd* pParent) : CSkinDialog(CURLCopyDlg::IDD, pParent)
{
}

void CURLCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INCLUDE_SELF, m_wndIncludeSelf);
	DDX_Control(pDX, IDC_MESSAGE, m_wndMessage);
	DDX_Text(pDX, IDC_URL_HOST, m_sHost);
	DDX_Text(pDX, IDC_URL_MAGNET, m_sMagnet);
	DDX_Text(pDX, IDC_URL_ED2K, m_sED2K);
}

void CURLCopyDlg::Add(const CPeerProjectFile* pFile)
{
	ASSERT( pFile != NULL );
	m_pFile = pFile;
}

/////////////////////////////////////////////////////////////////////////////
// CURLCopyDlg message handlers

BOOL CURLCopyDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( L"CURLCopyDlg", IDI_WEB_URL );

	m_wndIncludeSelf.ShowWindow( ( Network.IsListening() && m_pFile->m_sURL.IsEmpty() ) ? SW_SHOW : SW_HIDE );

	OnIncludeSelf();

	return TRUE;
}

void CURLCopyDlg::OnIncludeSelf()
{
	UpdateData();

	const BOOL bIncludeSelf = m_wndIncludeSelf.GetCheck();

	CString strURN, strIncludeSelfURN, strTemp;

	if ( m_pFile->m_oTiger && m_pFile->m_oSHA1 )
		strURN = _T("xt=urn:bitprint:") + m_pFile->m_oSHA1.toString() + '.' + m_pFile->m_oTiger.toString();
	else if ( m_pFile->m_oSHA1 )
		strURN = _T("xt=") + m_pFile->m_oSHA1.toUrn();
	else if ( m_pFile->m_oTiger )
		strURN = _T("xt=") + m_pFile->m_oTiger.toUrn();

	if ( m_pFile->m_oED2K )
	{
		strTemp = _T("xt=") + m_pFile->m_oED2K.toUrn();
		if ( ! strURN.IsEmpty() ) strURN += _T("&");
		strURN += strTemp;
	}

	BOOL bAddTracker( FALSE );
	if ( m_pFile->m_oBTH && strURN.IsEmpty() )	// BitTorrent Magnet
	{
		strURN = _T("xt=") + m_pFile->m_oBTH.toUrn();
		bAddTracker = TRUE;
	}

	m_sMagnet = strURN;

	if ( m_pFile->m_nSize != 0 && m_pFile->m_nSize != SIZE_UNKNOWN )
	{
		CString strSize;
		strSize.Format( _T("xl=%I64u"), m_pFile->m_nSize );

		if ( ! m_sMagnet.IsEmpty() ) m_sMagnet += _T("&");
		m_sMagnet += strSize;
	}

	if ( ! m_pFile->m_sName.IsEmpty() )
	{
		CString strName = URLEncode( m_pFile->m_sName );

		if ( ! m_sMagnet.IsEmpty() ) m_sMagnet += _T("&");
		m_sMagnet += ( strURN.IsEmpty() ? _T("kt=") : _T("dn=") ) + strName;
	}

	m_sMagnet = _T("magnet:?") + m_sMagnet;

	if ( bAddTracker )
	{
		if ( CDownload* pDownload = Downloads.FindByBTH( m_pFile->m_oBTH ) )
		{
			strTemp = URLEncode( (LPCTSTR)pDownload->m_pTorrent.GetTrackerAddress() );
			if ( strTemp.GetLength() > 24 )
				m_sMagnet += _T("&tr=") + strTemp;
		}
	}

	CString strSelf = bIncludeSelf ? m_pFile->GetURL( Network.m_pHost.sin_addr, htons( Network.m_pHost.sin_port ) ) : _T("");

	if ( bIncludeSelf && ! strSelf.IsEmpty() )
		m_sMagnet += _T("&xs=") + URLEncode( strSelf );
	else if ( ! m_pFile->m_sURL.IsEmpty() )
		m_sMagnet += _T("&xs=") + URLEncode( m_pFile->m_sURL );

	if ( m_pFile->m_oSHA1 )
	{
		m_sGnutella.Format( _T("gnutella://%s/"), (LPCTSTR)m_pFile->m_oSHA1.toUrn() );

		if ( ! m_pFile->m_sName.IsEmpty() )
			m_sGnutella += URLEncode( m_pFile->m_sName ) + _T("/");
	}

	if ( m_pFile->m_oED2K &&
		( m_pFile->m_nSize != 0 && m_pFile->m_nSize != SIZE_UNKNOWN ) &&
		! m_pFile->m_sName.IsEmpty() )
	{
		m_sED2K.Format( _T("ed2k://|file|%s|%I64u|%s|/"),
			(LPCTSTR)URLEncode( m_pFile->m_sName ),
			m_pFile->m_nSize,
			(LPCTSTR)m_pFile->m_oED2K.toString() );

		if ( bIncludeSelf )
			m_sED2K += _T("|sources,") + HostToString( &Network.m_pHost ) + _T("|/");

			//CString strURL;	// Obsolete
			//strURL.Format( _T("%s:%i"), (LPCTSTR)CString( inet_ntoa( Network.m_pHost.sin_addr ) ), htons( Network.m_pHost.sin_port ) );
			//m_sED2K += _T("|sources,") + strURL + _T("|/");
	}

	if ( bIncludeSelf )
		m_sHost = strSelf;
	else if ( ! m_pFile->m_sURL.IsEmpty() )
		m_sHost = m_pFile->m_sURL;
	else
		m_sHost.Empty();

	UpdateData( FALSE );
}

HBRUSH CURLCopyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd && pWnd != &m_wndMessage )
	{
		TCHAR szName[32];
		GetClassName( pWnd->GetSafeHwnd(), szName, 32 );

		if ( _tcsicmp( szName, _T("Static") ) == 0 )
		{
			pDC->SetTextColor( Colors.m_crTextLink );
			pDC->SelectObject( &theApp.m_gdiFontLine );
		}
	}

	return hbr;
}

BOOL CURLCopyDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	GetCursorPos( &point );

	for ( pWnd = GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
	{
		TCHAR szName[32];
		GetClassName( pWnd->GetSafeHwnd(), szName, 32 );

		if ( _tcsicmp( szName, _T("Static") ) == 0 && pWnd != &m_wndMessage )
		{
			CRect rc;
			pWnd->GetWindowRect( &rc );

			if ( rc.PtInRect( point ) )
			{
				CString strText;
				pWnd->GetWindowText( strText );

				if ( ! strText.IsEmpty() )
				{
					SetCursor( theApp.LoadCursor( IDC_HAND ) );
					return TRUE;
				}
			}
		}
	}

	return CSkinDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CURLCopyDlg::OnStnClickedUrlHost()
{
	UpdateData();

	if ( m_sHost.IsEmpty() )
		return;

	theApp.SetClipboard( m_sHost, TRUE );

	CSkinDialog::OnOK();
}

void CURLCopyDlg::OnStnClickedUrlMagnet()
{
	UpdateData();

	if ( m_sMagnet.IsEmpty() )
		return;

	theApp.SetClipboard( m_sMagnet, TRUE );

	CSkinDialog::OnOK();
}

void CURLCopyDlg::OnStnClickedUrlEd2k()
{
	UpdateData();

	if ( m_sED2K.IsEmpty() )
		return;

	theApp.SetClipboard( m_sED2K, TRUE );

	CSkinDialog::OnOK();
}
