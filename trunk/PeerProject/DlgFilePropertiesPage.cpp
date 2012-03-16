//
// DlgFilePropertiesPage.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2012
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
#include "DlgFilePropertiesPage.h"
#include "DlgFilePropertiesSheet.h"
#include "Library.h"
#include "SharedFile.h"
#include "ShellIcons.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNAMIC(CFilePropertiesPage, CPropertyPageAdv)

BEGIN_MESSAGE_MAP(CFilePropertiesPage, CPropertyPageAdv)
	//{{AFX_MSG_MAP(CFilePropertiesPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesPage property page

CFilePropertiesPage::CFilePropertiesPage(UINT nIDD) : CPropertyPageAdv( nIDD )
{
	//{{AFX_DATA_INIT(CFilePropertiesPage)
	//}}AFX_DATA_INIT
}

CFilePropertiesPage::~CFilePropertiesPage()
{
}

void CFilePropertiesPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPageAdv::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilePropertiesPage)
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesPage helper functions

CLibraryFile* CFilePropertiesPage::GetFile()
{
	CLibraryList* pList = GetList();
	if ( pList->GetCount() != 1 ) return NULL;
	CQuickLock oLock( Library.m_pSection );
	CLibraryFile* pFile = Library.LookupFile( pList->GetHead() );
	if ( pFile != NULL ) return pFile;
	PostMessage( WM_CLOSE );
	return NULL;
}

CLibraryList* CFilePropertiesPage::GetList() const
{
	CFilePropertiesSheet* pSheet = (CFilePropertiesSheet*)GetParent();
	return &pSheet->m_pList;
}

/////////////////////////////////////////////////////////////////////////////
// CFilePropertiesPage message handlers

BOOL CFilePropertiesPage::OnInitDialog()
{
	CPropertyPageAdv::OnInitDialog();

	CSingleLock oLock( &Library.m_pSection, TRUE );
	if ( CLibraryFile* pFile = GetFile() )
	{
		if ( CWnd* pNameWnd = GetDlgItem( IDC_FILE_NAME ) )
		{
			if ( Settings.General.LanguageRTL )
			{
				CRect rc, rcPage;
				pNameWnd->GetWindowRect( &rc );
				GetWindowRect( &rcPage );
				pNameWnd->MoveWindow( rcPage.right - rc.right,
					rc.top - rcPage.top, rc.Width(), rc.Height(), FALSE );
				pNameWnd->ModifyStyleEx( WS_EX_RTLREADING, WS_EX_LTRREADING, 0 );
			}
			pNameWnd->SetWindowText( pFile->m_sName );
		}
		m_nIcon = ShellIcons.Get( pFile->GetPath(), 48 );

		oLock.Unlock();
	}
	else
	{
		oLock.Unlock();
		if ( CLibraryList* pList = GetList() )
		{
			if ( CWnd* pNameWnd = GetDlgItem( IDC_FILE_NAME ) )
			{
				if ( Settings.General.LanguageRTL )
				{
					CRect rc, rcPage;
					pNameWnd->GetWindowRect( &rc );
					GetWindowRect( &rcPage );
					pNameWnd->MoveWindow( rcPage.right - rc.right,
						rc.top - rcPage.top, rc.Width(), rc.Height(), FALSE );
					pNameWnd->ModifyStyleEx( 0, WS_EX_RTLREADING, 0 );
				}
				CString strMessage;
				strMessage.Format( LoadString( IDS_LIBRARY_METADATA_EDIT ), pList->GetCount() );
				pNameWnd->SetWindowText( strMessage );
			}
			//m_nIcon = SHI_EXECUTABLE;
		}
	}

	return TRUE;
}
