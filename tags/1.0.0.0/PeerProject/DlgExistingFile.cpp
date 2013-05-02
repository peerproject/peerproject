//
// DlgExistingFile.cpp
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
#include "Settings.h"
#include "PeerProject.h"
#include "Library.h"
#include "SharedFile.h"
#include "DlgExistingFile.h"
#include "Colors.h"
#include "WndMain.h"
#include "WndLibrary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNAMIC(CExistingFileDlg, CSkinDialog)

BEGIN_MESSAGE_MAP(CExistingFileDlg, CSkinDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_ACTION_0, OnAction0)
	ON_BN_CLICKED(IDC_ACTION_1, OnAction1)
	ON_BN_CLICKED(IDC_ACTION_2, OnAction2)
END_MESSAGE_MAP()

//CExistingFileDlg::Action CExistingFileDlg::CheckExisting(const CPeerProjectURL* pURL)
//{
//	// Check files inside torrent
//	if ( pURL->m_pTorrent )
//	{
//		for ( POSITION pos = pURL->m_pTorrent->m_pFiles.GetHeadPosition() ; pos ; )
//		{
//			CBTInfo::CBTFile* pBTFile = pURL->m_pTorrent->m_pFiles.GetNext( pos );
//
//			return CheckExisting( static_cast< const CPeerProjectFile* >( pBTFile ), bInteracive );
//		}
//	}
//
//	// Check as common file
//	return CheckExisting( static_cast< const CPeerProjectFile* >( pURL ) );
//}

CExistingFileDlg::Action CExistingFileDlg::CheckExisting(const CPeerProjectFile* pFile)
{
	CSingleLock pLock( &Library.m_pSection );
	if ( ! pLock.Lock( 1000 ) )
		return Download;

	CLibraryFile* pLibFile = LibraryMaps.LookupFileByHash( pFile );
	if ( pLibFile == NULL )
		return Download;

	const DWORD nIndex = pLibFile->m_nIndex;

	CExistingFileDlg dlg( pLibFile );

	pLock.Unlock();

	dlg.DoModal();

	if ( dlg.m_nAction == 0 )
	{
		if ( CLibraryWnd* pLibrary = CLibraryWnd::GetLibraryWindow() )
		{
			pLock.Lock();
			if ( CLibraryFile* pLibFile1 = Library.LookupFile( nIndex ) )
				pLibrary->Display( pLibFile1 );

			pLock.Unlock();
		}
	}

	return dlg.GetResult();
}

/////////////////////////////////////////////////////////////////////////////
// CExistingFileDlg dialog

CExistingFileDlg::CExistingFileDlg(const CLibraryFile* pFile, CWnd* pParent, bool bDuplicateSearch)
	: CSkinDialog( CExistingFileDlg::IDD, pParent )
	, m_nAction  ( 0 )
{
	m_sName = pFile->m_sName;

	if ( ! bDuplicateSearch )
	{
		if ( pFile->m_oSHA1 && pFile->m_oTiger )
			m_sURN	= _T("bitprint:") + pFile->m_oSHA1.toString() + '.' + pFile->m_oTiger.toString();
		else if ( pFile->m_oSHA1 )
			m_sURN = pFile->m_oSHA1.toString();
		else if ( pFile->m_oTiger )
			m_sURN = pFile->m_oTiger.toUrn();

		m_sComments  = pFile->m_sComments;
		m_bAvailable = pFile->IsAvailable() ? TRI_FALSE : TRI_UNKNOWN;
	}
	else if ( pFile->m_oMD5 )
	{
		m_sURN = pFile->m_oMD5.toUrn();
		m_bAvailable = TRI_TRUE;
	}
}

INT_PTR CExistingFileDlg::DoModal()
{
	INT_PTR ret = CSkinDialog::DoModal();
	if ( ret != IDOK )
		m_nAction = 3;
	return ret;
}

void CExistingFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDOK, m_wndOK);
	DDX_Control(pDX, IDC_FILE_NAME, m_wndName);
	DDX_Text(pDX, IDC_FILE_NAME, m_sName);
	DDX_Text(pDX, IDC_FILE_URN, m_sURN);
	DDX_Text(pDX, IDC_FILE_COMMENTS, m_sComments);
	DDX_Control(pDX, IDC_FILE_COMMENTS, m_wndComments);
	DDX_Control(pDX, IDC_MESSAGE_AVAILABLE, m_wndMessageAvailable);
	DDX_Control(pDX, IDC_MESSAGE_DELETED, m_wndMessageDeleted);
	DDX_Control(pDX, IDC_MESSAGE_DUPLICATES, m_wndMessageDuplicates);
	DDX_Control(pDX, IDC_ACTION_0, m_wndLocate);
	DDX_Control(pDX, IDC_ACTION_1, m_wndDownload);
	DDX_Control(pDX, IDC_ACTION_2, m_wndDontDownload);
	DDX_Radio(pDX, IDC_ACTION_0, m_nAction);
}

/////////////////////////////////////////////////////////////////////////////
// CExistingFileDlg message handlers

BOOL CExistingFileDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	SkinMe( L"CExistingFileDlg", IDR_DOWNLOADSFRAME );

	if ( m_bAvailable == TRI_UNKNOWN )
		m_nAction = 1;
	else if ( m_bAvailable == TRI_TRUE )
		m_nAction = 0;

	UpdateData( FALSE );

	m_wndComments.ShowWindow( ! m_sComments.IsEmpty() ? SW_SHOW : SW_HIDE );
	m_wndMessageAvailable.ShowWindow( m_bAvailable == TRI_FALSE ? SW_SHOW : SW_HIDE );
	m_wndMessageDeleted.ShowWindow( m_bAvailable != TRI_UNKNOWN ? SW_HIDE : SW_SHOW );
	m_wndMessageDuplicates.ShowWindow( m_bAvailable == TRI_TRUE ? SW_SHOW : SW_HIDE );
	m_wndDownload.ShowWindow( m_bAvailable == TRI_TRUE ? SW_HIDE : SW_SHOW );
	m_wndDontDownload.ShowWindow( m_bAvailable == TRI_TRUE ? SW_HIDE : SW_SHOW );
	m_wndLocate.EnableWindow( m_bAvailable != TRI_UNKNOWN );

	return TRUE;
}

HBRUSH CExistingFileDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CSkinDialog::OnCtlColor( pDC, pWnd, nCtlColor );

	if ( pWnd == &m_wndName || pWnd == &m_wndMessageAvailable || pWnd == &m_wndMessageDeleted )
		pDC->SelectObject( &theApp.m_gdiFontBold );
	if ( pWnd == &m_wndComments && m_bAvailable == TRI_UNKNOWN )
		pDC->SetTextColor( Colors.m_crTextAlert );
	if ( pWnd == &m_wndMessageDuplicates )
		pDC->SetTextColor( Colors.m_crTextAlert );

	return hbr;
}

void CExistingFileDlg::OnAction0()
{
	if ( m_wndLocate.IsWindowEnabled() )
		m_wndOK.EnableWindow( TRUE );
}

void CExistingFileDlg::OnAction1()
{
	m_wndOK.EnableWindow( TRUE );
}

void CExistingFileDlg::OnAction2()
{
	m_wndOK.EnableWindow( TRUE );
}

void CExistingFileDlg::OnOK()
{
	UpdateData();
	if ( m_bAvailable == TRI_UNKNOWN && m_nAction == 0 ) return;
	CSkinDialog::OnOK();
}
