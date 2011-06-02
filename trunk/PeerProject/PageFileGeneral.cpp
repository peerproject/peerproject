//
// PageFileGeneral.cpp
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
#include "PeerProject.h"
#include "Settings.h"
#include "PageFileGeneral.h"
#include "Library.h"
#include "SharedFolder.h"
#include "SharedFile.h"
#include "ShellIcons.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CFileGeneralPage, CFilePropertiesPage)

BEGIN_MESSAGE_MAP(CFileGeneralPage, CFilePropertiesPage)
	//{{AFX_MSG_MAP(CFileGeneralPage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFileGeneralPage property page

CFileGeneralPage::CFileGeneralPage()
	: CFilePropertiesPage( CFileGeneralPage::IDD )
	, m_sType()
	, m_sPath()
	, m_sSize()
	, m_sModified()
	, m_sIndex()
	, m_sSHA1()
	, m_sTiger()
	, m_sED2K()
	, m_sMD5()
{
}

CFileGeneralPage::~CFileGeneralPage()
{
}

void CFileGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CFilePropertiesPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileGeneralPage)
	DDX_Text(pDX, IDC_FILE_TYPE, m_sType);
	DDX_Text(pDX, IDC_FILE_PATH, m_sPath);
	DDX_Text(pDX, IDC_FILE_SIZE, m_sSize);
	DDX_Text(pDX, IDC_FILE_MODIFIED, m_sModified);
	DDX_Text(pDX, IDC_FILE_INDEX, m_sIndex);
	DDX_Text(pDX, IDC_FILE_SHA1, m_sSHA1);
	DDX_Text(pDX, IDC_FILE_TIGER, m_sTiger);
	DDX_Text(pDX, IDC_FILE_ED2K, m_sED2K);
	DDX_Text(pDX, IDC_FILE_MD5, m_sMD5);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CFileGeneralPage message handlers

BOOL CFileGeneralPage::OnInitDialog()
{
	CFilePropertiesPage::OnInitDialog();

	{
		CQuickLock oLock( Library.m_pSection );

		CLibraryFile* pFile = GetFile();
		if ( ! pFile ) return TRUE;

		m_sPath = pFile->GetFolder();
		m_sType = ShellIcons.GetTypeString( pFile->m_sName );
		m_sSize.Format( _T("%s  (%I64i)"), Settings.SmartVolume( pFile->GetSize() ), pFile->GetSize() );
		m_sIndex.Format( _T("# %lu"), pFile->m_nIndex );

		m_sSHA1	 = pFile->m_oSHA1.toShortUrn();
		m_sTiger = pFile->m_oTiger.toShortUrn();
		m_sED2K	 = pFile->m_oED2K.toShortUrn();
		m_sMD5	 = pFile->m_oMD5.toShortUrn();

		if ( m_sSHA1.IsEmpty() && m_sED2K.IsEmpty() && m_sTiger.IsEmpty() && m_sMD5.IsEmpty() )
			LoadString( m_sSHA1, IDS_NO_URN_AVAILABLE );

		CString strDate, strTime;
		SYSTEMTIME pTime;

		FileTimeToSystemTime( &pFile->m_pTime, &pTime );
		SystemTimeToTzSpecificLocalTime( NULL, &pTime, &pTime );

		GetDateFormat( LOCALE_USER_DEFAULT, DATE_LONGDATE, &pTime, NULL, strDate.GetBuffer( 64 ), 64 );
		GetTimeFormat( LOCALE_USER_DEFAULT, TIME_FORCE24HOURFORMAT, &pTime, NULL, strTime.GetBuffer( 64 ), 64 );
		strDate.ReleaseBuffer();
		strTime.ReleaseBuffer();

		m_sModified = strDate + _T(", ") + strTime;
	}

	UpdateData( FALSE );

	return TRUE;
}

void CFileGeneralPage::OnOK()
{
	// Nothing to update now

	UpdateData();

	CFilePropertiesPage::OnOK();
}
