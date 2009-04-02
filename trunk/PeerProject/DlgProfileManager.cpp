//
// DlgProfileManager.cpp
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
#include "Settings.h"
#include "GProfile.h"
#include "Skin.h"

#include "DlgProfileManager.h"
#include "PageProfileIdentity.h"
#include "PageProfileContact.h"
#include "PageProfileProfile.h"
#include "PageProfileBio.h"
#include "PageProfileAvatar.h"
#include "PageProfileFavourites.h"
#include "PageProfileFiles.h"
#include "PageProfileCertificate.h"
#include "PageSettingsRich.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CProfileManagerDlg, CSettingsSheet)

BEGIN_MESSAGE_MAP(CProfileManagerDlg, CSettingsSheet)
	//{{AFX_MSG_MAP(CProfileManagerDlg)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDRETRY, OnApply)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg construction

CProfileManagerDlg::CProfileManagerDlg(CWnd* pParent) : CSettingsSheet( pParent, IDS_USER_PROFILE )
{
	m_nTopMargin = 52;
}

CProfileManagerDlg::~CProfileManagerDlg()
{
}

/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg operations

BOOL CProfileManagerDlg::Run(LPCTSTR pszWindow)
{
	CProfileManagerDlg pSheet;
	BOOL bResult = ( pSheet.DoModal( pszWindow ) == IDOK );
	return bResult;
}

INT_PTR CProfileManagerDlg::DoModal(LPCTSTR pszWindow)
{
	CIdentityProfilePage	pIdentity;
	CContactProfilePage		pContact;
	CProfileProfilePage		pProfile;
	CBioProfilePage			pBio;
	CAvatarProfilePage		pAvatar;
	CFavouritesProfilePage	pFavourites;
	CFilesProfilePage		pFiles;
	CCertificateProfilePage	pCertificate;

	AddGroup( &pIdentity );
	AddPage( &pContact );
	AddPage( &pProfile );
	AddPage( &pBio );
	AddPage( &pAvatar );
	AddGroup( &pFavourites );
	AddPage( &pFiles );
	AddGroup( &pCertificate );

	if ( pszWindow ) SetActivePage( GetPage( pszWindow ) );

	INT_PTR nReturn = CSettingsSheet::DoModal();

	return nReturn;
}

void CProfileManagerDlg::AddPage(CSettingsPage* pPage)
{
	CString strCaption = Skin.GetDialogCaption( CString( pPage->GetRuntimeClass()->m_lpszClassName ) );
	CSettingsSheet::AddPage( pPage, strCaption.GetLength() ? (LPCTSTR)strCaption : NULL );
}

void CProfileManagerDlg::AddGroup(CSettingsPage* pPage)
{
	if ( pPage->IsKindOf( RUNTIME_CLASS(CRichSettingsPage) ) )
	{
		CString strCaption = ((CRichSettingsPage*)pPage)->m_sCaption;
		CSettingsSheet::AddGroup( pPage, strCaption );
	}
	else
	{
		CString strName = pPage->GetRuntimeClass()->m_lpszClassName;
		CString strCaption = Skin.GetDialogCaption( strName );
		CSettingsSheet::AddGroup( pPage, strCaption.GetLength() ? (LPCTSTR)strCaption : NULL );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProfileManagerDlg message handlers

BOOL CProfileManagerDlg::OnInitDialog()
{
	CSettingsSheet::OnInitDialog();

	m_bmHeader.LoadBitmap( IDB_WIZARD );

	SkinMe( NULL, IDR_MAINFRAME );

	return TRUE;
}

void CProfileManagerDlg::DoPaint(CDC& dc)
{
	CRect rc;
	GetClientRect( &rc );

	BITMAP pInfo;
	m_bmHeader.GetBitmap( &pInfo );

	CDC mdc;
	mdc.CreateCompatibleDC( &dc );
	CBitmap* pOldBitmap = (CBitmap*)mdc.SelectObject( &m_bmHeader );
	dc.BitBlt( 0, 0, pInfo.bmWidth, pInfo.bmHeight, &mdc, 0, 0, SRCCOPY );
	mdc.SelectObject( pOldBitmap );
	mdc.DeleteDC();

	CSettingsSheet::DoPaint( dc );
}

void CProfileManagerDlg::OnOK()
{
	CSettingsSheet::OnOK();
	MyProfile.Save();
}

void CProfileManagerDlg::OnApply()
{
	CSettingsSheet::OnApply();
	MyProfile.Save();
}
