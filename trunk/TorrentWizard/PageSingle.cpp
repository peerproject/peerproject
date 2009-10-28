//
// PageSingle.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008
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
#include "PageSingle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSinglePage, CWizardPage)

BEGIN_MESSAGE_MAP(CSinglePage, CWizardPage)
	//{{AFX_MSG_MAP(CSinglePage)
	ON_BN_CLICKED(IDC_BROWSE_FILE, OnBrowseFile)
	ON_WM_XBUTTONDOWN()
	ON_WM_DROPFILES()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSinglePage property page

CSinglePage::CSinglePage() : CWizardPage(CSinglePage::IDD)
{
	//{{AFX_DATA_INIT(CSinglePage)
	m_sFileName = _T("");
	m_sFileSize = _T("");
	//m_sMagnet = _T("");
	//}}AFX_DATA_INIT
}

CSinglePage::~CSinglePage()
{
}

void CSinglePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSinglePage)
	DDX_Text(pDX, IDC_FILE_NAME, m_sFileName);
	DDX_Text(pDX, IDC_FILE_SIZE, m_sFileSize);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CSinglePage message handlers

void CSinglePage::OnReset() 
{
	m_sFileName.Empty();
	m_sFileSize.Empty();
	UpdateData( FALSE );
}

BOOL CSinglePage::OnSetActive() 
{
	if ( m_sFileName.IsEmpty() ) SetTimer( 1, 25, NULL );
	SetWizardButtons( PSWIZB_BACK | PSWIZB_NEXT );
	this->DragAcceptFiles(TRUE);

	return CWizardPage::OnSetActive();
}

void CSinglePage::OnTimer(UINT_PTR /*nIDEvent*/) 
{
	KillTimer( 1 );	
	PostMessage( WM_COMMAND, MAKELONG( IDC_BROWSE_FILE, BN_CLICKED ) );
}

void CSinglePage::OnDropFiles( HDROP hDropInfo )
{
	CString sFilename;

	LPWSTR pszFile = sFilename.GetBuffer( _MAX_PATH );
	DragQueryFile( hDropInfo, 0, pszFile, _MAX_PATH );

	m_sFileName = sFilename;

	DragFinish( hDropInfo );

	HANDLE hFile = CreateFile( m_sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD nLow, nHigh;
		nLow = GetFileSize( hFile, &nHigh );
		CloseHandle( hFile );
		
		QWORD nSize = ( (QWORD)nHigh << 32 ) + (QWORD)nLow;
		m_sFileSize = SmartSize( nSize );

		UpdateData( FALSE );
	}
}

void CSinglePage::OnBrowseFile() 
{
	UpdateData( TRUE );
	
	CFileDialog dlg( TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("All Files|*.*||"), this );
	if ( dlg.DoModal() != IDOK ) return;
	
	m_sFileName = dlg.GetPathName();
	
	HANDLE hFile = CreateFile( m_sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD nLow, nHigh;
		nLow = GetFileSize( hFile, &nHigh );
		CloseHandle( hFile );
		
		QWORD nSize = ( (QWORD)nHigh << 32 ) + (QWORD)nLow;
		m_sFileSize = SmartSize( nSize );
	}
	else
	{
		CString strFormat, strMessage;
		strFormat.LoadString( IDS_SINGLE_CANT_OPEN );
		strMessage.Format( strFormat, (LPCTSTR)m_sFileName );
		AfxMessageBox( strMessage, MB_ICONEXCLAMATION );
		
		m_sFileName.Empty();
	}
	
	UpdateData( FALSE );
}

// ToDo: Display Magnet Link in new textbox on file-load

LRESULT CSinglePage::OnWizardBack() 
{
	return IDD_WELCOME_PAGE;
}

LRESULT CSinglePage::OnWizardNext() 
{
	UpdateData();
	
	if ( m_sFileName.IsEmpty() ||
		 GetFileAttributes( m_sFileName ) == 0xFFFFFFFF )
	{
		AfxMessageBox( IDS_SINGLE_NEED_FILE, MB_ICONEXCLAMATION );
		return -1;
	}
	
	return IDD_TRACKER_PAGE;
}

void CSinglePage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
	else if ( nButton == 2 )
		GetSheet()->PressButton( PSBTN_NEXT );
}
