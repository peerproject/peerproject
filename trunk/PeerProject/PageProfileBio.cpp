//
// PageProfileBio.cpp : implementation file
//
// This file is part of PeerProject (peerproject.org) � 2008-2010
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
#include "GProfile.h"
#include "XML.h"
#include "PageProfileBio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// Filename

IMPLEMENT_DYNCREATE(CBioProfilePage, CSettingsPage)

BEGIN_MESSAGE_MAP(CBioProfilePage, CSettingsPage)
	//{{AFX_MSG_MAP(CBioProfilePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBioProfilePage property page

CBioProfilePage::CBioProfilePage() : CSettingsPage( CBioProfilePage::IDD )
{
	//{{AFX_DATA_INIT(CBioProfilePage)
	//}}AFX_DATA_INIT
}

CBioProfilePage::~CBioProfilePage()
{
}

void CBioProfilePage::DoDataExchange(CDataExchange* pDX)
{
	CSettingsPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBioProfilePage)
	DDX_Control(pDX, IDC_PROFILE_BIO, m_wndText);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
// CBioProfilePage message handlers

BOOL CBioProfilePage::OnInitDialog()
{
	CSettingsPage::OnInitDialog();

	if ( CXMLElement* pNotes = MyProfile.GetXML( _T("notes") ) )
		m_wndText.SetWindowText( pNotes->GetValue() );

	UpdateData( FALSE );

	return TRUE;
}

void CBioProfilePage::OnOK()
{
	if ( CXMLElement* pNotes = MyProfile.GetXML( _T("notes"), TRUE ) )
	{
		CString str;
		m_wndText.GetWindowText( str );

		if ( ! str.IsEmpty() )
			pNotes->SetValue( str );
		else
			pNotes->Delete();
	}

	CSettingsPage::OnOK();
}
