//
// PageCommandline.cpp
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
#include "TorrentBuilder.h"
#include "PageCommandline.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCommandlinePage, CWizardPage)

BEGIN_MESSAGE_MAP(CCommandlinePage, CWizardPage)
	ON_BN_CLICKED(IDC_ABORT, OnAbort)
	ON_BN_CLICKED(IDC_TORRENT_OPEN, OnTorrentOpen)
	ON_BN_CLICKED(IDC_TORRENT_SEED, OnTorrentSeed)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCommandlinePage property page

CCommandlinePage::CCommandlinePage() : CWizardPage(CCommandlinePage::IDD), m_pBuilder( NULL )
{
}

CCommandlinePage::~CCommandlinePage()
{
	if ( m_pBuilder ) 
		delete m_pBuilder;
}

void CCommandlinePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommandlinePage)
	DDX_Control(pDX, IDC_ABORT, m_wndAbort);
	DDX_Control(pDX, IDC_TORRENT_NAME, m_wndTorrentName);
	DDX_Control(pDX, IDC_TORRENT_OPEN, m_wndTorrentOpen);
	DDX_Control(pDX, IDC_TORRENT_SEED, m_wndTorrentSeed);
	DDX_Control(pDX, IDC_SPEED_MESSAGE, m_wndSpeedMessage);
	DDX_Control(pDX, IDC_SPEED_SLIDER, m_wndSpeed);
	DDX_Control(pDX, IDC_SPEED_SLOW, m_wndSpeedSlow);
	DDX_Control(pDX, IDC_SPEED_FAST, m_wndSpeedFast);
	DDX_Control(pDX, IDC_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_FILE_NAME, m_wndFileName);
	DDX_Control(pDX, IDC_DONE_2, m_wndDone2);
	DDX_Control(pDX, IDC_DONE_1, m_wndDone1);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CCommandlinePage message handlers

BOOL CCommandlinePage::OnInitDialog() 
{
	CWizardPage::OnInitDialog();
	m_wndSpeed.SetRange( 0, 5 );
	m_wndSpeed.SetPos( 3 );
	return TRUE;
}

BOOL CCommandlinePage::OnSetActive() 
{
	SetTimer( 2, 25, NULL );
	return CWizardPage::OnSetActive();
}

void CCommandlinePage::Start()
{
	if ( m_pBuilder ) delete m_pBuilder;
	m_pBuilder = new CTorrentBuilder();

	CString sFolder;

	CString strFile = theApp.m_sCommandLineSourceFile;
			
	if ( LPCTSTR pszSlash = _tcsrchr( strFile, '\\' ) )
	{
		m_sDestinationFile = pszSlash + 1;
		m_sDestinationFile += _T(".torrent");
	}

	m_pBuilder->SetOutputFile( theApp.m_sCommandLineDestination + '\\' + m_sDestinationFile );
		
	m_pBuilder->AddTrackerURL( theApp.m_sCommandLineTracker );
		
	m_pBuilder->SetComment( _T("") );
		
	m_pBuilder->AddFile( theApp.m_sCommandLineSourceFile );
	
	m_pBuilder->Start();
	
	SetTimer( 1, 200, NULL );
	PostMessage( WM_TIMER, 1 );
	
	
	m_wndDone1.ShowWindow( SW_HIDE );
	m_wndDone2.ShowWindow( SW_HIDE );
	m_wndTorrentName.ShowWindow( SW_HIDE );
	m_wndTorrentOpen.ShowWindow( SW_HIDE );
	m_wndTorrentSeed.ShowWindow( SW_HIDE );

	m_wndAbort.ShowWindow( SW_SHOW );
	m_wndSpeedMessage.ShowWindow( SW_SHOW );
	m_wndSpeedSlow.ShowWindow( SW_SHOW );
	m_wndSpeedFast.ShowWindow( SW_SHOW );
	m_wndSpeed.ShowWindow( SW_SHOW );
	m_wndProgress.SetPos( 0 );
	m_wndProgress.SetRange( 0, 1 );

	SetWizardButtons( 0 );
}

void CCommandlinePage::OnTimer(UINT_PTR nIDEvent) 
{
	BOOL bFinished = FALSE;
	
	if ( nIDEvent == 2 )
	{
		KillTimer( 2 );
		Start();
		return;
	}

	if ( m_pBuilder != NULL )
	{
		CString str1, str2;
		DWORD nPos, nLen;
		
		if ( m_pBuilder->GetTotalProgress( nPos, nLen ) )
		{
			m_wndProgress.SetRange32( 0, nLen );
			// m_wndProgress.SetRange( 0, nLen );
			m_wndProgress.SetPos( nPos );
		}
		
		if ( m_pBuilder->GetCurrentFile( str1 ) )
		{
			m_wndFileName.GetWindowText( str2 );
			if ( str1 != str2 ) m_wndFileName.SetWindowText( str1 );
		}
		
		if ( m_pBuilder->IsRunning() ) return;
		
		bFinished = m_pBuilder->IsFinished();
	
		str1.Empty();
		m_pBuilder->GetMessageString( str1 );
		m_wndFileName.SetWindowText( str1 );
		
		m_pBuilder->Stop();
		delete m_pBuilder;
		m_pBuilder = NULL;
	}
	
	KillTimer( 1 );
	
	m_wndAbort.ShowWindow( SW_HIDE );
	m_wndSpeedMessage.ShowWindow( SW_HIDE );
	m_wndSpeedSlow.ShowWindow( SW_HIDE );
	m_wndSpeedFast.ShowWindow( SW_HIDE );
	m_wndSpeed.ShowWindow( SW_HIDE );
	
	m_wndTorrentName.SetWindowText( m_sDestinationFile );

	
	if ( bFinished )
	{
		m_wndDone1.ShowWindow( SW_SHOW );
		m_wndTorrentName.ShowWindow( SW_SHOW );
		m_wndTorrentOpen.ShowWindow( SW_SHOW );
		m_wndTorrentSeed.ShowWindow( SW_SHOW );
	}
	else
	{
		m_wndProgress.SetPos( 0 );
	}
	
	m_wndDone2.ShowWindow( SW_SHOW );
	
	SetWizardButtons( PSWIZB_FINISH );
}

void CCommandlinePage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( m_pBuilder != NULL )
	{
		switch ( m_wndSpeed.GetPos() )
		{
		case 0:
			m_pBuilder->SetPriority( THREAD_PRIORITY_IDLE );
			break;
		case 1:
			m_pBuilder->SetPriority( THREAD_PRIORITY_LOWEST );
			break;
		case 2:
			m_pBuilder->SetPriority( THREAD_PRIORITY_BELOW_NORMAL );
			break;
		case 3:
			m_pBuilder->SetPriority( THREAD_PRIORITY_NORMAL );
			break;
		case 4:
			m_pBuilder->SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );
			break;
		case 5:
			m_pBuilder->SetPriority( THREAD_PRIORITY_HIGHEST );
			break;
		}
	}
	
	CWizardPage::OnHScroll( nSBCode, nPos, pScrollBar );
}

BOOL CCommandlinePage::OnWizardFinish() 
{
	return m_pBuilder == NULL;
}

void CCommandlinePage::OnAbort() 
{
	CWaitCursor pCursor;
	if ( m_pBuilder != NULL ) m_pBuilder->Stop();
}

void CCommandlinePage::OnTorrentOpen() 
{
	ShellExecute( GetSafeHwnd(), _T("open"),
		theApp.m_sCommandLineDestination, NULL, NULL, SW_SHOWNORMAL );
}

void CCommandlinePage::OnTorrentSeed() 
{
	ShellExecute( GetSafeHwnd(), NULL, 
		theApp.m_sCommandLineDestination + '\\' + m_sDestinationFile, NULL, NULL, SW_SHOWNORMAL );
}