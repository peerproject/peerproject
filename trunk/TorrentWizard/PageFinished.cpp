//
// PageFinished.cpp
//
// This file is part of PeerProject Torrent Wizard (peerproject.org) � 2008-2011
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
#include "PageFinished.h"
#include "PageOutput.h"
#include "PageComment.h"
#include "PageTracker.h"
#include "PagePackage.h"
#include "PageSingle.h"
#include "PageExpert.h"
#include "PageWelcome.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CFinishedPage, CWizardPage)

BEGIN_MESSAGE_MAP(CFinishedPage, CWizardPage)
	//{{AFX_MSG_MAP(CFinishedPage)
	ON_BN_CLICKED(IDC_ABORT, OnAbort)
	ON_BN_CLICKED(IDC_TORRENT_COPY, OnTorrentCopy)
	ON_BN_CLICKED(IDC_TORRENT_OPEN, OnTorrentOpen)
	ON_BN_CLICKED(IDC_TORRENT_SEED, OnTorrentSeed)
	ON_WM_XBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFinishedPage property page

CFinishedPage::CFinishedPage() : CWizardPage(CFinishedPage::IDD)
{
	//{{AFX_DATA_INIT(CFinishedPage)
	m_pBuilder = NULL;
	//}}AFX_DATA_INIT
}

CFinishedPage::~CFinishedPage()
{
	if ( m_pBuilder ) delete m_pBuilder;
}

void CFinishedPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFinishedPage)
	DDX_Control(pDX, IDC_ABORT, m_wndAbort);
	DDX_Control(pDX, IDC_TORRENT_NAME, m_wndTorrentName);
	DDX_Control(pDX, IDC_TORRENT_COPY, m_wndTorrentCopy);
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
// CFinishedPage message handlers

BOOL CFinishedPage::OnInitDialog()
{
	CWizardPage::OnInitDialog();
	m_wndSpeed.SetRange( 0, 5 );
	m_wndSpeed.SetPos( 3 );
	return TRUE;
}

BOOL CFinishedPage::OnSetActive()
{
	SetTimer( 2, 25, NULL );
	return CWizardPage::OnSetActive();
}

void CFinishedPage::Start()
{
	if ( m_pBuilder ) delete m_pBuilder;
	m_pBuilder = new CTorrentBuilder();

	GET_PAGE( COutputPage, pOutput );
	m_pBuilder->SetOutputFile( pOutput->m_sFolder + '\\' + pOutput->m_sName );
	if ( pOutput->m_bAutoPieces )
		m_pBuilder->SetPieceSize( -1 );
	else
		m_pBuilder->SetPieceSize( pOutput->m_nPieceIndex );
	m_pBuilder->Enable( pOutput->m_bSHA1, pOutput->m_bED2K, pOutput->m_bMD5 );

	GET_PAGE( CTrackerPage, pTracker );
	m_pBuilder->AddTrackerURL( pTracker->m_sTracker );
	m_pBuilder->AddTrackerURL2( pTracker->m_sTracker2 );

	GET_PAGE( CCommentPage, pComment );
	m_pBuilder->SetComment( pComment->m_sComment );

	GET_PAGE( CWelcomePage, pWelcome );

	if ( pWelcome->m_nType == 0 )
	{
		GET_PAGE( CSinglePage, pSingle );
		m_pBuilder->AddFile( pSingle->m_sFileName );
	}
	else if ( pWelcome->m_nType == 1 )
	{
		GET_PAGE( CPackagePage, pPackage );

		for ( int nFile = 0 ; nFile < pPackage->m_wndList.GetItemCount() ; nFile++ )
		{
			m_pBuilder->AddFile( pPackage->m_wndList.GetItemText( nFile, 0 ) );
		}
	}
	else
	{
		GET_PAGE( CExpertPage, pExpert );

		for ( int nFile = 0 ; nFile < pExpert->m_wndList.GetItemCount() ; nFile++ )
		{
			m_pBuilder->AddFile( pExpert->m_wndList.GetItemText( nFile, 0 ) );
		}
	}

	m_pBuilder->Start();

	SetTimer( 1, 200, NULL );
	PostMessage( WM_TIMER, 1 );

	m_wndDone1.ShowWindow( SW_HIDE );
	m_wndDone2.ShowWindow( SW_HIDE );
	m_wndTorrentName.ShowWindow( SW_HIDE );
	m_wndTorrentCopy.ShowWindow( SW_HIDE );
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

void CFinishedPage::OnTimer(UINT_PTR nIDEvent)
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
			if ( str1 != str2 )
				m_wndFileName.SetWindowText( str1 );
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

	GET_PAGE( COutputPage, pOutput );
	m_wndTorrentName.SetWindowText( pOutput->m_sFolder + '\\' + pOutput->m_sName );

	if ( bFinished )
	{
		m_wndDone1.ShowWindow( SW_SHOW );
		m_wndTorrentName.ShowWindow( SW_SHOW );
		m_wndTorrentCopy.ShowWindow( SW_SHOW );
		m_wndTorrentOpen.ShowWindow( SW_SHOW );
		m_wndTorrentSeed.ShowWindow( SW_SHOW );
	}
	else
	{
		m_wndProgress.SetPos( 0 );
	}

	m_wndDone2.ShowWindow( SW_SHOW );

	SetWizardButtons( PSWIZB_BACK | PSWIZB_FINISH );
}

void CFinishedPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CFinishedPage::OnXButtonDown(UINT /*nFlags*/, UINT nButton, CPoint /*point*/)
{
	if ( nButton == 1 )
		GetSheet()->PressButton( PSBTN_BACK );
}

LRESULT CFinishedPage::OnWizardBack()
{
	if ( m_pBuilder == NULL )
	{
		GetSheet()->DoReset();
		return IDD_WELCOME_PAGE;
	}

	return -1;
}

BOOL CFinishedPage::OnWizardFinish()
{
	return m_pBuilder == NULL;
}

void CFinishedPage::OnAbort()
{
	CWaitCursor pCursor;
	if ( m_pBuilder != NULL ) m_pBuilder->Stop();
}

void CFinishedPage::OnTorrentCopy()
{
	if ( OpenClipboard() )
	{
		CString strText;
		m_wndTorrentName.GetWindowText( strText );
		OSVERSIONINFO pVersion;
		pVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx( &pVersion );

		if ( pVersion.dwPlatformId == VER_PLATFORM_WIN32_NT )
		{
			HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, strText.GetLength() * 2 + 1 );
			LPVOID pMem = GlobalLock( hMem );
			CopyMemory( pMem, (LPCTSTR)strText.GetBuffer(), strText.GetLength() * 2 + 1 );
			GlobalUnlock( hMem );

			EmptyClipboard();
			SetClipboardData( CF_UNICODETEXT, hMem );
			CloseClipboard();
		}
		else
		{
			int nLen = WideCharToMultiByte( CP_ACP,  0, (LPCTSTR)strText.GetBuffer(), -1, NULL, 0, NULL, NULL );
			LPSTR pStr	= new CHAR[ nLen + 1 ];
			WideCharToMultiByte( CP_ACP, 0, (LPCTSTR)strText.GetBuffer(), -1, pStr, nLen, NULL, NULL );
			pStr[ nLen ] = 0;
			HANDLE hMem = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, nLen + 1 );
			LPVOID pMem = GlobalLock( hMem );
			CopyMemory( pMem, pStr, nLen + 1 );
			GlobalUnlock( hMem );

			EmptyClipboard();
			SetClipboardData( CF_TEXT, hMem );
			CloseClipboard();
			delete [] pStr;
		}
	}
}

void CFinishedPage::OnTorrentOpen()
{
//	GET_PAGE( COutputPage, pOutput );
//	ShellExecute( GetSafeHwnd(), _T("open"), pOutput->m_sFolder, NULL, NULL, SW_SHOWNORMAL );

	CString strPath;
	m_wndTorrentName.GetWindowText( strPath );
	ShellExecute( GetSafeHwnd(), NULL, _T("Explorer.exe"), "/select, " + strPath, NULL, SW_SHOWNORMAL );
}

void CFinishedPage::OnTorrentSeed()
{
	CString strText;
	m_wndTorrentName.GetWindowText( strText );
	ShellExecute( GetSafeHwnd(), NULL, strText, NULL, NULL, SW_SHOWNORMAL );
}
