//
// DlgURLCopy.cpp
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
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

#include "StdAfx.h"
#include "PeerProject.h"
#include "CoolInterface.h"
#include "DlgURLCopy.h"
#include "Transfer.h"
#include "Network.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CURLCopyDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CURLCopyDlg, CSkinDialog)
	//{{AFX_MSG_MAP(CURLCopyDlg)
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_INCLUDE_SELF, OnIncludeSelf)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CURLCopyDlg dialog

CURLCopyDlg::CURLCopyDlg(CWnd* pParent) : CSkinDialog(CURLCopyDlg::IDD, pParent)
{
}

void CURLCopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CURLCopyDlg)
	DDX_Control(pDX, IDC_INCLUDE_SELF, m_wndIncludeSelf);
	DDX_Control(pDX, IDC_MESSAGE, m_wndMessage);
	DDX_Text(pDX, IDC_URL_HOST, m_sHost);
	DDX_Text(pDX, IDC_URL_MAGNET, m_sMagnet);
	DDX_Text(pDX, IDC_URL_ED2K, m_sED2K);
	//}}AFX_DATA_MAP
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

	SkinMe( NULL, IDI_WEB_URL );

	m_sHost = m_pFile->m_sURL;

	m_wndIncludeSelf.ShowWindow( ( Network.IsListening() && m_sHost.IsEmpty() )
		? SW_SHOW : SW_HIDE );

	OnIncludeSelf();

	return TRUE;
}

void CURLCopyDlg::OnIncludeSelf()
{
	CString strURN, strIncludeSelfURN, strTemp;

	if ( m_pFile->m_oTiger && m_pFile->m_oSHA1 )
	{
		strURN	= _T("xt=urn:bitprint:")
				+ m_pFile->m_oSHA1.toString() + '.'
				+ m_pFile->m_oTiger.toString();
	}
	else if ( m_pFile->m_oSHA1 )
	{
		strURN = _T("xt=") + m_pFile->m_oSHA1.toUrn();
	}
	else if ( m_pFile->m_oTiger )
	{
		strURN = _T("xt=") + m_pFile->m_oTiger.toUrn();
	}

	if ( m_pFile->m_oED2K )
	{
		strTemp = _T("xt=") + m_pFile->m_oED2K.toUrn();
		if ( strURN.GetLength() ) strURN += _T("&");
		strURN += strTemp;
	}

	m_sMagnet = strURN;

	if ( m_pFile->m_nSize != 0 && m_pFile->m_nSize != SIZE_UNKNOWN )
	{
		CString strSize;

		strSize.Format( _T("xl=%I64i"),
			m_pFile->m_nSize );

		if ( m_sMagnet.GetLength() ) m_sMagnet += _T("&");
		m_sMagnet += strSize;
	}

	if ( m_pFile->m_sName.GetLength() )
	{
		CString strName = URLEncode( m_pFile->m_sName );

		if ( m_sMagnet.GetLength() ) m_sMagnet += _T("&");
		if ( strURN.GetLength() )
			m_sMagnet += _T("dn=") + strName;
		else
			m_sMagnet += _T("kt=") + strName;
	}

	m_sMagnet = _T("magnet:?") + m_sMagnet;

	if ( m_wndIncludeSelf.GetCheck() )
	{
		CString strURL = m_pFile->GetURL( Network.m_pHost.sin_addr,
			htons( Network.m_pHost.sin_port ) );
		if ( strURL.GetLength() )
			m_sMagnet += _T("&xs=") + URLEncode( strURL );
	}

	if ( m_pFile->m_oSHA1 )
	{
		m_sGnutella.Format( _T("gnutella://%s/"),
			(LPCTSTR)m_pFile->m_oSHA1.toUrn() );

		if ( m_pFile->m_sName.GetLength() )
		{
			m_sGnutella += URLEncode( m_pFile->m_sName )
						+ _T("/");
		}
	}

	if ( m_pFile->m_oED2K && 
		( m_pFile->m_nSize != 0 && m_pFile->m_nSize != SIZE_UNKNOWN ) &&
		m_pFile->m_sName.GetLength() )
	{
		m_sED2K.Format( _T("ed2k://|file|%s|%I64i|%s|/"),
			(LPCTSTR)URLEncode( m_pFile->m_sName ),
			m_pFile->m_nSize,
			(LPCTSTR)m_pFile->m_oED2K.toString() );

		if ( m_wndIncludeSelf.GetCheck() )
		{
			CString strURL2;

			strURL2.Format ( _T("%s:%i"),
					(LPCTSTR)CString( inet_ntoa( Network.m_pHost.sin_addr ) ),
					htons( Network.m_pHost.sin_port ) );

			m_sED2K += _T("|sources,")
					+ strURL2
					+ _T("|/");
		}
	}

	UpdateData( FALSE );
}

HBRUSH CURLCopyDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if ( pWnd && pWnd != &m_wndMessage )
	{
		TCHAR szName[32];
		GetClassName( pWnd->GetSafeHwnd(), szName, 32 );

		if ( ! _tcsicmp( szName, _T("Static") ) )
		{
			pDC->SetTextColor( CoolInterface.m_crTextLink );
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

		if ( ! _tcsicmp( szName, _T("Static") ) && pWnd != &m_wndMessage )
		{
			CString strText;
			CRect rc;

			pWnd->GetWindowRect( &rc );

			if ( rc.PtInRect( point ) )
			{
				pWnd->GetWindowText( strText );

				if ( strText.GetLength() )
				{
					SetCursor( theApp.LoadCursor( IDC_HAND ) );
					return TRUE;
				}
			}
		}
	}

	return CSkinDialog::OnSetCursor( pWnd, nHitTest, message );
}

void CURLCopyDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	ClientToScreen( &point );

	for ( CWnd* pWnd = GetWindow( GW_CHILD ) ; pWnd ; pWnd = pWnd->GetNextWindow() )
	{
		TCHAR szName[32];
		GetClassName( pWnd->GetSafeHwnd(), szName, 32 );

		if ( ! _tcsicmp( szName, _T("Static") ) && pWnd != &m_wndMessage )
		{
			CRect rc;
			pWnd->GetWindowRect( &rc );

			if ( rc.PtInRect( point ) )
			{
				CString strURL;

				pWnd->GetWindowText( strURL );
				if ( strURL.IsEmpty() ) return;

				SetClipboardText( strURL );

				CSkinDialog::OnOK();
				return;
			}
		}
	}

	CSkinDialog::OnLButtonDown( nFlags, point );
}

BOOL CURLCopyDlg::SetClipboardText(CString& strText)
{
	if ( ! AfxGetMainWnd()->OpenClipboard() ) return FALSE;

	EmptyClipboard();

		CT2CW pszWide( (LPCTSTR)strText );
		HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, ( wcslen(pszWide) + 1 ) * sizeof(WCHAR) );
		if ( hMem )
		{
			LPVOID pMem = GlobalLock( hMem );
			if ( pMem )
			{
				CopyMemory( pMem, pszWide, ( wcslen(pszWide) + 1 ) * sizeof(WCHAR) );
				GlobalUnlock( hMem );
				SetClipboardData( CF_UNICODETEXT, hMem );
			}
		}

	CloseClipboard();

	return TRUE;
}