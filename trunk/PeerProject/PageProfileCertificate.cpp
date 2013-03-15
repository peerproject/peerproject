//
// PageProfileCertificate.cpp
//
// This file is part of PeerProject (peerproject.org) © 2008-2010
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
#include "PageProfileCertificate.h"
#include "GProfile.h"
#include "XML.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Debug

IMPLEMENT_DYNCREATE(CCertificateProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CCertificateProfilePage, CSettingsPage)
	ON_BN_CLICKED(IDC_GUID_CREATE, OnGuidCreate)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCertificateProfilePage property page

CCertificateProfilePage::CCertificateProfilePage()
	: CSettingsPage( CCertificateProfilePage::IDD )
	, m_sGUID	( _T("") )
	, m_sGUIDBT	( _T("") )
//	, m_sTime	( _T("") )
{
}

CCertificateProfilePage::~CCertificateProfilePage()
{
}

void CCertificateProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GUID, m_sGUID);
	DDX_Text(pDX, IDC_GUID_BT, m_sGUIDBT);
//	DDX_Text(pDX, IDC_GUID_TIME, m_sTime);
}

/////////////////////////////////////////////////////////////////////////////
// CCertificateProfilePage message handlers

BOOL CCertificateProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	wchar_t szGUID[39];
	Hashes::Guid GUIDtmp( MyProfile.oGUID );
	szGUID[ StringFromGUID2( *(GUID*)&GUIDtmp[0], szGUID, 39 ) - 2 ] = 0;
	m_sGUID = (CString)&szGUID[1];

	Hashes::BtGuid GUIDbt( MyProfile.oGUIDBT );
	m_sGUIDBT = GUIDbt.toString();

	UpdateData( FALSE );

	return TRUE;
}

void CCertificateProfilePage::OnGuidCreate()
{
	MyProfile.Create();

	UpdateData( TRUE );

	wchar_t szGUID[39];
	Hashes::Guid GUIDtmp( MyProfile.oGUID );
	szGUID[ StringFromGUID2( *(GUID*)&GUIDtmp[ 0 ], szGUID, 39 ) - 2 ] = 0;
	m_sGUID = (CString)&szGUID[1];

	Hashes::BtGuid GUIDbt( MyProfile.oGUIDBT );
	m_sGUIDBT = GUIDbt.toString();

	UpdateData( FALSE );
}

void CCertificateProfilePage::OnOK()
{
	UpdateData();

	CSettingsPage::OnOK();
}
